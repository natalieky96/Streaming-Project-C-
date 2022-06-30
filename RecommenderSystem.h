
#ifndef RECOMMENDERSYSTEM_H
#define RECOMMENDERSYSTEM_H
#include "RSUser.h"
#include <map>

class RecommenderSystem
{
 public:

  //explicit RecommenderSystem()
  /**
   * adds a new movie to the system
   * @param name name of movie
   * @param year year it was made
   * @param features features for movie
   * @return shared pointer for movie in system
   */
  sp_movie add_movie (const std::string &name, int year, const
  std::vector<double> &features);

  /**
   * a function that calculates the movie with highest score based on
   * movie features
   * @param ranks user ranking to use for algorithm
   * @return shared pointer to movie in system
   */
  sp_movie recommend_by_content (const RSUser &user);

  /**
   * a function that calculates the movie with highest predicted score
   * based on ranking of other movies
   * @param ranks user ranking to use for algorithm
   * @param k
   * @return shared pointer to movie in system
   */
  sp_movie recommend_by_cf (const RSUser &user, int k);

  /**
   * Predict a user rating for a movie given argument using item cf procedure
   * with k most similar movies.
   * @param user_rankings: ranking to use
   * @param movie: movie to predict
   * @param k:
   * @return score based on algorithm as described in pdf
   */
  double predict_movie_score (const RSUser &user, const sp_movie &movie,
                              int k);

  /**
   * gets a shared pointer to movie in system
   * @param name name of movie
   * @param year year movie was made
   * @return shared pointer to movie in system
   */
  sp_movie get_movie (const std::string &name, int year) const;

  //<< operator return to ostraem the right
  friend std::ostream &operator<< (std::ostream &s, const RecommenderSystem
  &rs);

 private:

  //compare func struct uses to order the map
  struct compare_func
  {
      bool operator() (const sp_movie &movie1, const sp_movie &movie2) const
      {
        return *movie1 < *movie2;
      }
  };

  std::map<sp_movie, std::vector<double>, compare_func> _movies_map;

  /**
   * Calculate the rank mean of the elemnts from the rank map
   * @param rs_user the user we want to calculate its rank map mean
   * @return the mean
   */
  double get_users_rank_map_mean (const RSUser &rs_user) const;

  /**
   *
   * @param rs_user
   * @param mean
   * @return
   */
  std::vector<double> find_user_recommended_vec (const RSUser &rs_user) const;

  /*
   * The function get a vector and it's size and return its norm
   */
  double get_norm (const std::vector<double> &vec, int n) const;
  /*
  * the function gets 2 vectors and calculate its product
  */
  double vector_multiply (const std::vector<double> &vec1, const
  std::vector<double> &vec2);

  double calculate_similarity (const std::vector<double> &rec_vec, const
  std::vector<double> &features);

  /*
   * helper for the function get_predict_rank, the function calculate the final
   * value of the movie predictible rank
   */
  double get_predict_helper (const RSUser &user, std::map<double, sp_movie>
  &similarities, int k);

};

#endif //RECOMMENDERSYSTEM_H
