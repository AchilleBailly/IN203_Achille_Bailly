#include "TimeMeasure.hpp"
#include "contexte.hpp"
#include "graphisme/src/SDL2/sdl2.hpp"
#include "grille.hpp"
#include "individu.hpp"
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <random>
#include <vector>

void màjStatistique_bug(épidémie::Grille &grille,
                        std::vector<épidémie::Individu> const &individus) {
    auto [largeur, hauteur] = grille.dimension();
    auto &statistiques = grille.getStatistiques();
    std::vector<bool> seen(statistiques.size());

    for (auto const &personne : individus) {
        auto pos = personne.position();

        std::size_t index = pos.x + pos.y * largeur;
        if (!seen[index]) {
            statistiques[index]
                .nombre_contaminant_grippé_et_contaminé_par_agent = 0;
            statistiques[index]
                .nombre_contaminant_seulement_contaminé_par_agent = 0;
            statistiques[index].nombre_contaminant_seulement_grippé = 0;
            seen[index] = true;
        }
        if (personne.aGrippeContagieuse()) {
            if (personne.aAgentPathogèneContagieux()) {
                statistiques[index]
                    .nombre_contaminant_grippé_et_contaminé_par_agent += 1;
            } else {
                statistiques[index].nombre_contaminant_seulement_grippé += 1;
            }
        } else {
            if (personne.aAgentPathogèneContagieux()) {
                statistiques[index]
                    .nombre_contaminant_seulement_contaminé_par_agent += 1;
            }
        }
    }
    for (int i = 0; i < seen.size(); i++) {
        if (!seen[i]) {
            statistiques[i].nombre_contaminant_grippé_et_contaminé_par_agent =
                0;
            statistiques[i].nombre_contaminant_seulement_contaminé_par_agent =
                0;
            statistiques[i].nombre_contaminant_seulement_grippé = 0;
        }
    }
}

void màjStatistique(épidémie::Grille &grille,
                    std::vector<épidémie::Individu> const &individus) {
    for (auto &statistique : grille.getStatistiques()) {
        statistique.nombre_contaminant_grippé_et_contaminé_par_agent = 0;
        statistique.nombre_contaminant_seulement_contaminé_par_agent = 0;
        statistique.nombre_contaminant_seulement_grippé = 0;
    }
    auto [largeur, hauteur] = grille.dimension();
    auto &statistiques = grille.getStatistiques();
    for (auto const &personne : individus) {
        auto pos = personne.position();

        std::size_t index = pos.x + pos.y * largeur;
        if (personne.aGrippeContagieuse()) {
            if (personne.aAgentPathogèneContagieux()) {
                statistiques[index]
                    .nombre_contaminant_grippé_et_contaminé_par_agent += 1;
            } else {
                statistiques[index].nombre_contaminant_seulement_grippé += 1;
            }
        } else {
            if (personne.aAgentPathogèneContagieux()) {
                statistiques[index]
                    .nombre_contaminant_seulement_contaminé_par_agent += 1;
            }
        }
    }
}

void afficheSimulation(sdl2::window &écran, épidémie::Grille const &grille,
                       std::size_t jour) {
    auto [largeur_écran, hauteur_écran] = écran.dimensions();
    auto [largeur_grille, hauteur_grille] = grille.dimension();
    auto const &statistiques = grille.getStatistiques();
    sdl2::font fonte_texte("./graphisme/src/data/Lato-Thin.ttf", 18);
    écran.cls({0x00, 0x00, 0x00});
    // Affichage de la grille :
    std::uint16_t stepX = largeur_écran / largeur_grille;
    unsigned short stepY = (hauteur_écran - 50) / hauteur_grille;
    double factor = 255. / 15.;

    for (unsigned short i = 0; i < largeur_grille; ++i) {
        for (unsigned short j = 0; j < hauteur_grille; ++j) {
            auto const &stat = statistiques[i + j * largeur_grille];
            int valueGrippe =
                stat.nombre_contaminant_grippé_et_contaminé_par_agent +
                stat.nombre_contaminant_seulement_grippé;
            int valueAgent =
                stat.nombre_contaminant_grippé_et_contaminé_par_agent +
                stat.nombre_contaminant_seulement_contaminé_par_agent;
            std::uint16_t origx = i * stepX;
            std::uint16_t origy = j * stepY;
            std::uint8_t red =
                valueGrippe > 0 ? 127 + std::uint8_t(std::min(
                                            128., 0.5 * factor * valueGrippe))
                                : 0;
            std::uint8_t green =
                std::uint8_t(std::min(255., factor * valueAgent));
            std::uint8_t blue =
                std::uint8_t(std::min(255., factor * valueAgent));
            écran << sdl2::rectangle({origx, origy}, {stepX, stepY},
                                     {red, green, blue}, true);
        }
    }

    écran << sdl2::texte("Carte population grippée", fonte_texte, écran,
                         {0xFF, 0xFF, 0xFF, 0xFF})
                 .at(largeur_écran / 2, hauteur_écran - 20);
    écran << sdl2::texte(std::string("Jour : ") + std::to_string(jour),
                         fonte_texte, écran, {0xFF, 0xFF, 0xFF, 0xFF})
                 .at(0, hauteur_écran - 20);
    écran << sdl2::flush;
}

void simulation_process0(bool affiche) {
    int nbp;
    MPI_Comm_size(MPI_COMM_WORLD, &nbp);
    MPI_Comm splitComm;
    MPI_Comm_split(MPI_COMM_WORLD, 0, -1, &splitComm);
    MPI_Datatype MPI_StatParCase;
    MPI_Type_contiguous(3, MPI_INT, &MPI_StatParCase);
    MPI_Type_commit(&MPI_StatParCase);

    TimeMeasuring t0, t1, t5;

    MPI_Request req;

    constexpr const unsigned int largeur_écran = 1280, hauteur_écran = 720;
    sdl2::window écran("Simulation épidémie de grippe",
                       {largeur_écran, hauteur_écran});
    sdl2::event_queue queue;
    bool quitting = false;
    size_t jours_écoulés = 0;
    int dim_x, dim_y;
    MPI_Status status;
    MPI_Recv(&dim_x, 1, MPI_INT, 1, 42, MPI_COMM_WORLD, &status);
    MPI_Recv(&dim_y, 1, MPI_INT, 1, 42, MPI_COMM_WORLD, &status);
    std::cout << dim_x << ", " << dim_y << "\n";
    épidémie::Grille grille(dim_x * dim_y);
    grille.m_statistiques.reserve(dim_x * dim_y);

    while (!quitting) {

        t5.setStart();
        if (affiche) {
            t0.setStart();
            // fake send to say that we are ready to accept a message
            MPI_Send(&jours_écoulés, 1, MPI_INT, 1, 41, MPI_COMM_WORLD);

            MPI_Recv(&jours_écoulés, 1, MPI_INT, 1, 42, MPI_COMM_WORLD,
                     &status);
            MPI_Recv(grille.m_statistiques.data(), dim_x * dim_y,
                     MPI_StatParCase, 1, 42, MPI_COMM_WORLD, &status);
            t0.setEnd();

            t1.setStart();
            afficheSimulation(écran, grille, jours_écoulés);
            t1.setEnd();
        }
        t5.setEnd();

        auto events = queue.pull_events();
        for (const auto &e : events) {
            if (e->kind_of_event() == sdl2::event::quit) {
                quitting = true;
                for (int i = 1; i < nbp; i++)
                    MPI_Ssend(&quitting, 1, MPI_INT, i, 10, MPI_COMM_WORLD);
            }
        }
    }

    std::cout << "Itération du process d'affichage : "
              << t5.getAverageDuration().count() << "\n";
    std::cout << "Affichage : " << t1.getAverageDuration().count() << "\n";
    std::cout << "Réception des données : " << t0.getAverageDuration().count()
              << "\n";
}

void simulation_process1(bool affiche) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm splitComm;
    MPI_Comm_split(MPI_COMM_WORLD, 1, rank - 1, &splitComm);
    int nbp;
    MPI_Comm_size(splitComm, &nbp);
    MPI_Comm_rank(splitComm, &rank);

    // creating a new MPI data type for struct StatistiqueParCase
    MPI_Datatype MPI_StatParCase;
    MPI_Type_contiguous(3, MPI_INT, &MPI_StatParCase);
    MPI_Type_commit(&MPI_StatParCase);

    TimeMeasuring t0, t1, t2, t3, t4, t5;

    unsigned int graine_aléatoire = 1;
    std::uniform_real_distribution<double> porteur_pathogène(0., 1.);

    épidémie::ContexteGlobal contexte;
    // contexte.déplacement_maximal = 1; <= Si on veut moins de brassage
    contexte.taux_population = nbp * 100'000;
    // contexte.taux_population = 1'000;
    contexte.interactions.β = 60.;
    std::vector<épidémie::Individu> population;
    const int pop_per_process = contexte.taux_population / nbp;
    population.reserve(pop_per_process);
    épidémie::Grille grille{contexte.taux_population};
    épidémie::Grille grille_tot{contexte.taux_population};

    auto [largeur_grille, hauteur_grille] = grille.dimension();
    if (rank == 0) {
        MPI_Ssend(&largeur_grille, 1, MPI_INT, 0, 42, MPI_COMM_WORLD);
        MPI_Ssend(&hauteur_grille, 1, MPI_INT, 0, 42, MPI_COMM_WORLD);
    }

    // L'agent pathogène n'évolue pas et reste donc constant...
    épidémie::AgentPathogène agent(graine_aléatoire++);

    // Initialisation de la population initiale :
    // bien faire attention à donner la même graine suivant le rang du processus
    // que si on était en séquentiel
    for (int i = rank * pop_per_process; i < (rank + 1) * pop_per_process;
         ++i) {
        std::default_random_engine motor(100 * (i + 1));
        population.emplace_back(graine_aléatoire + i, contexte.espérance_de_vie,
                                contexte.déplacement_maximal);
        population.back().setPosition(largeur_grille, hauteur_grille);
        if (porteur_pathogène(motor) < 0.2) {
            population.back().estContaminé(agent);
        }
    }

    std::size_t jours_écoulés = 0;
    int jour_apparition_grippe = 0;
    int nombre_immunisés_grippe_tot = (contexte.taux_population * 23) / 100;
    int nombre_immunisés_grippe = 0;
    // encore une fois, comme on a choisi de répartir la population
    // séquentillement
    // et pas en "round robin", certains processus n'ont pas d'immunisés
    if ((rank + 1) * pop_per_process <= nombre_immunisés_grippe_tot)
        nombre_immunisés_grippe = pop_per_process;
    else if (rank * pop_per_process <= nombre_immunisés_grippe_tot)
        nombre_immunisés_grippe = nombre_immunisés_grippe_tot % pop_per_process;

    bool quitting = false;

    std::ofstream output("Courbe.dat");
    output << "# jours_écoulés \t nombreTotalContaminésGrippe \t "
              "nombreTotalContaminésAgentPathogène()"
           << std::endl;

    épidémie::Grippe grippe(0);

    std::cout << "Début boucle épidémie" << std::endl << std::flush;
    MPI_Request req = MPI_REQUEST_NULL;
    MPI_Status status;
    int flag = true;
    while (!quitting) {
        t0.setStart();
        if (jours_écoulés % 365 == 0) // Si le premier Octobre (début de
                                      // l'année pour l'épidémie ;-) )
        {
            t1.setStart();
            grippe = épidémie::Grippe(jours_écoulés / 365);
            jour_apparition_grippe = grippe.dateCalculImportationGrippe();
            grippe.calculNouveauTauxTransmission();
            // 23% des gens sont immunisés. On prend les 23% premiers
            for (int ipersonne = 0; ipersonne < nombre_immunisés_grippe;
                 ++ipersonne) {
                population[ipersonne].devientImmuniséGrippe();
            }
            for (int ipersonne = nombre_immunisés_grippe;
                 ipersonne < int(pop_per_process); ++ipersonne) {
                population[ipersonne].redevientSensibleGrippe();
            }
            t1.setEnd();
        }

        if (jours_écoulés % 365 == std::size_t(jour_apparition_grippe) &&
            nombre_immunisés_grippe < pop_per_process) {
            // rebelotte, les 25 contaminés initiaux ne sont pas dans chaque
            // processus ...
            t2.setStart();
            if (nombre_immunisés_grippe + 25 <= pop_per_process &&
                nombre_immunisés_grippe != 0) {
                for (int ipersonne = nombre_immunisés_grippe;
                     ipersonne < nombre_immunisés_grippe + 25; ++ipersonne) {
                    population[ipersonne].estContaminé(grippe);
                }
            } else if (nombre_immunisés_grippe != 0) {
                for (int ipersonne = nombre_immunisés_grippe;
                     ipersonne < pop_per_process; ++ipersonne) {
                    population[ipersonne].estContaminé(grippe);
                }
            } else if ((rank - 1) * pop_per_process <=
                       nombre_immunisés_grippe_tot) {
                int to_contaminate =
                    25 - (pop_per_process -
                          nombre_immunisés_grippe_tot % pop_per_process);
                for (int ipersonne = 0; ipersonne < to_contaminate;
                     ++ipersonne) {
                    population[ipersonne].estContaminé(grippe);
                }
            }
            t2.setEnd();
        }

        // Mise à jour des statistiques pour les cases de la grille :
        t3.setStart();
        màjStatistique(grille, population);

        // on additionne les grilles "partielles" de chaque processus
        // pour que chacun ait la grille entière
        MPI_Allreduce(
            grille.m_statistiques.data(), grille_tot.m_statistiques.data(),
            3 * largeur_grille * hauteur_grille, MPI_INT, MPI_SUM, splitComm);
        grille = grille_tot;

        // pour tuer chaque proc quand l'affichage le dit (ne fonctionne pas
        // bien)
        MPI_Irecv(&quitting, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &req);
        if (quitting)
            break;

        t3.setEnd();
        // On parcout la population pour voir qui est contaminé et qui
        // ne l'est pas, d'abord pour la grippe puis pour l'agent
        // pathogène
        std::size_t compteur_grippe = 0, compteur_agent = 0, mouru = 0;
        t4.setStart();
        for (auto &personne : population) {
            if (omp_get_num_threads() == 16)
                std::cout << "OpenMP working !!\n";
            if (personne.testContaminationGrippe(grille, contexte.interactions,
                                                 grippe, agent)) {
                compteur_grippe++;
                personne.estContaminé(grippe);
            }
            if (personne.testContaminationAgent(grille, agent)) {
                compteur_agent++;
                personne.estContaminé(agent);
            }
            // On vérifie si il n'y a pas de personne qui veillissent de
            // veillesse et on génère une nouvelle personne si c'est le
            // cas.
            if (personne.doitMourir()) {
                mouru++;
                unsigned nouvelle_graine =
                    jours_écoulés +
                    personne.position().x * personne.position().y;
                personne = épidémie::Individu(nouvelle_graine,
                                              contexte.espérance_de_vie,
                                              contexte.déplacement_maximal);
                personne.setPosition(largeur_grille, hauteur_grille);
            }
            personne.veillirDUnJour();
            personne.seDéplace(grille);
        }
        t4.setEnd();

        /*std::cout << jours_écoulés << "\t" <<
           grille.nombreTotalContaminésGrippe()
           << "\t"
                  << grille.nombreTotalContaminésAgentPathogène() <<
           std::endl;*/

        if (rank == 0) {
            output << jours_écoulés << "\t"
                   << grille.nombreTotalContaminésGrippe() << "\t"
                   << grille.nombreTotalContaminésAgentPathogène() << std::endl;
        }
        jours_écoulés += 1;

        t0.setEnd();

        t5.setStart();
        if (affiche && rank == 0) {
            // comme avant, on envoit seulement si l'autre est prêt à recevoir
            MPI_Iprobe(0, 41, MPI_COMM_WORLD, &flag, &status);
            if (flag) {
                int dump;
                MPI_Recv(&dump, 1, MPI_INT, 0, 41, MPI_COMM_WORLD, &status);
                MPI_Send(&jours_écoulés, 1, MPI_INT, 0, 42, MPI_COMM_WORLD);
                MPI_Send(grille_tot.m_statistiques.data(),
                         largeur_grille * hauteur_grille, MPI_StatParCase, 0,
                         42, MPI_COMM_WORLD);
            }
        }

        t5.setEnd();
    }

    output.close();

    // Fin boucle temporelle
    if (rank == 0) {
        std::cout << "Itération du process de simu : "
                  << t0.getAverageDuration().count() << "\n";
        std::cout << "Premier jour de l'année : "
                  << t1.getAverageDuration().count() << "\n";
        std::cout << "Premier jour de grippe : "
                  << t2.getAverageDuration().count() << "\n";
        std::cout << "Calcul des stats de la grille : "
                  << t3.getAverageDuration().count() << "\n";
        std::cout << "Mise a jour de la grille : "
                  << t4.getAverageDuration().count() << "\n";
        std::cout << "Envoi des données au process 0 : "
                  << t5.getAverageDuration().count() << "\n";
    }
}

int main(int argc, char *argv[]) {
    // parse command-line
    bool affiche = true;
    for (int i = 0; i < argc; i++) {
        std::cout << i << " " << argv[i] << "\n";
        if (std::string("-nw") == argv[i])
            affiche = false;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm globComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
    int nbp;
    MPI_Comm_size(globComm, &nbp);
    if (nbp < 2) {
        std::cerr << "Choose more than 2 processes.\n";
        exit(-1);
    }
    int rank;
    MPI_Comm_rank(globComm, &rank);

    if (rank == 0) {
        sdl2::init(argc, argv);
        simulation_process0(affiche);
        sdl2::finalize();
    } else
        simulation_process1(affiche);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
