
#ifndef USER_H
#define USER_H
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "Movie.h"

class RecommenderSystem;
typedef std::unordered_map<sp_movie, double, hash_func, equal_func> rank_map;

using std::string;
using std::shared_ptr;

class RSUser
{
 public:
  /**
   * Constructor for the class
   */
  RSUser (string user_name, rank_map movies_ranks,
          shared_ptr<RecommenderSystem> recommender_system);
  /**
   * a getter for the user's name
   * @return the username
   */
  const string &get_name () const
  { return this->_user_name; }

  /**
   * function for adding a movie to the DB
   * @param movie a Movie object
   * @param features a vector of the movie's features
   * @param rate the user rate for this movie
   * @return true for success, false otherwise
   */
  void add_movie_to_rs (const std::string &name, int year,
                        const std::vector<double> &features,
                        double rate);

  /**
   * a getter for the ranks map
   * @return
   */
  const rank_map &get_ranks () const
  { return this->_movies_rank; }
  /**
   * returns a recommendation according to the movie's content
   * @return recommendation
   */
  sp_movie get_recommendation_by_content () const;

  /**
   * returns a recommendation according to the similarity recommendation method
   * @param k the number of the most similar movies to calculate by
   * @return recommendation
   */
  sp_movie get_recommendation_by_cf (int k) const;

  /**
   * predicts the score for a given movie
   * @param name the name of the movie
   * @param year the year the movie was created
   * @param k the parameter which represents the number of the most similar
   * movies to predict the score by
   * @return predicted score for the given movie
   */
  double
  get_prediction_score_for_movie (const std::string &name, int year, int k)
  const;

  /**
   * output stream operator
   * @param os the output stream
   * @param user the user
   * @return output stream
   */
  // TODO &operator<<
  friend std::ostream &operator<< (std::ostream &os, const RSUser &user);

 private:
  string _user_name;
  rank_map _movies_rank;
  shared_ptr<RecommenderSystem> _recommender_system;

};

#endif //USER_H
