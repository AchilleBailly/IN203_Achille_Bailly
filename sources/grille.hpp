#ifndef _EPIDEMIE_GRILLE_HPP_
#define _EPIDEMIE_GRILLE_HPP_
#include "contexte.hpp"
#include <array>
#include <cstdint>

struct StatistiqueParCase {
    int nombre_contaminant_seulement_grippé;
    int nombre_contaminant_seulement_contaminé_par_agent;
    int nombre_contaminant_grippé_et_contaminé_par_agent;
};

namespace épidémie {
class Grille {
  public:
    // Taille de la grille dépend de la taille de la population
    Grille(std::size_t population);

    std::vector<StatistiqueParCase> &getStatistiques() {
        return m_statistiques;
    }

    std::vector<StatistiqueParCase> const &getStatistiques() const {
        return m_statistiques;
    }

    std::size_t nombreTotalContaminésGrippe() const;

    std::size_t nombreTotalContaminésAgentPathogène() const;

    std::array<int, 2> dimension() const { return {m_dim_x, m_dim_y}; }

    int m_dim_x, m_dim_y;
    std::vector<StatistiqueParCase> m_statistiques;
};
} // namespace épidémie

#endif