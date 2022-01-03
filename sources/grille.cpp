#include "grille.hpp"

épidémie::Grille::Grille(std::size_t population) {
    int taille = int(sqrt(population));
    m_dim_x = (population + taille - 1) / taille;
    m_dim_y = taille;
    std::vector<épidémie::Grille::StatistiqueParCase>(m_dim_x * m_dim_y)
        .swap(m_statistiques);
}
//
std::size_t épidémie::Grille::nombreTotalContaminésGrippe() const {
    std::size_t total = 0;
    for (auto statistique : m_statistiques) {
        total += statistique.nombre_contaminant_grippé_et_contaminé_par_agent;
        total += statistique.nombre_contaminant_seulement_grippé;
    }
    return total;
}
//
std::size_t épidémie::Grille::nombreTotalContaminésAgentPathogène() const {
    std::size_t total = 0;
    for (auto statistique : m_statistiques) {
        total += statistique.nombre_contaminant_seulement_contaminé_par_agent;
        total += statistique.nombre_contaminant_grippé_et_contaminé_par_agent;
    }
    return total;
}
//
épidémie::Grille::StatistiqueParCase
operator+(const épidémie::Grille::StatistiqueParCase &c1,
          const épidémie::Grille::StatistiqueParCase &c2) {
    return {c1.nombre_contaminant_seulement_grippé +
                c2.nombre_contaminant_seulement_grippé,
            c1.nombre_contaminant_seulement_contaminé_par_agent +
                c2.nombre_contaminant_seulement_contaminé_par_agent,
            c1.nombre_contaminant_grippé_et_contaminé_par_agent +
                c2.nombre_contaminant_grippé_et_contaminé_par_agent};
}