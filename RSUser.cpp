
// don't change those includes
#include "RSUser.h"
#include "RecommenderSystem.h"
using std::shared_ptr;
using std::make_shared;
#define USER_NAME "name: "

//todo: check if this how we initiliaze this
RSUser::RSUser (string user_name, rank_map movie_ranks,
                shared_ptr<RecommenderSystem> recommender_system)
    : _user_name (user_name),
      _movies_rank
          (movie_ranks), _recommender_system (recommender_system)
{
}

void RSUser::add_movie_to_rs (const std::string &name, int year,
                              const std::vector<double> &features, double rate)
{
  sp_movie new_movie = _recommender_system->add_movie (name, year, features);
  _movies_rank[new_movie] = rate;
}

sp_movie RSUser::get_recommendation_by_content () const
{
  return _recommender_system->recommend_by_content (*this);
}

sp_movie RSUser::get_recommendation_by_cf (int k) const
{
  return _recommender_system->recommend_by_cf (*this, k);
}

double
RSUser::get_prediction_score_for_movie (const std::string &name, int year,
                                        int k) const
{
  sp_movie movie = _recommender_system->get_movie (name, year);
  return _recommender_system->predict_movie_score (*this, movie, k);
}

std::ostream &operator<< (std::ostream &os, const RSUser &user)
{
  os << USER_NAME << user.get_name () << std::endl;
  os << (*user._recommender_system) << std::endl;
  return os;
}
