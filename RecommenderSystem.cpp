#include <map>
#include "RecommenderSystem.h"
#include <cmath>
#define DEFAULT_VALUE_COMPARE -10

sp_movie RecommenderSystem::add_movie (const std::string &name, int year, const
std::vector<double> &features)
{
  Movie new_movie (name, year);
  sp_movie new_movie_ptr = std::make_shared<Movie> (new_movie);
  _movies_map[new_movie_ptr] = features;
  return new_movie_ptr;
}

double RecommenderSystem::get_users_rank_map_mean (const RSUser &rs_user) const
{
  rank_map user_rank_map = rs_user.get_ranks ();
  double sum = 0;
  int num_of_movies = 0;
  rank_map::iterator it;
  for (it = user_rank_map.begin (); it != user_rank_map.end (); it++)
  {
    if (it->second >= 0)
    {
      num_of_movies += 1;
      sum += it->second;
    }
  }
  //we cant divide by zero(0)
  if (num_of_movies == 0)
  {
    return 0;
  }
  return sum / num_of_movies;
}

std::vector<double> RecommenderSystem::find_user_recommended_vec
    (const RSUser &rs_user)
const
{
  int curr_normal_rank;
  double rank_mean = get_users_rank_map_mean (rs_user);
  std::vector<double> curr_feature, rcmnd_vec;
  int num_movies_seen_user = rs_user.get_ranks ().size ();
  int num_of_movies = _movies_map.size ();
  for (int i = 0; i < num_of_movies; i++)
  {
    rcmnd_vec.push_back (0);
  }
  rank_map user_rank_map = rs_user.get_ranks ();
  rank_map::iterator it;
  for (it = user_rank_map.begin (); it != user_rank_map.end (); it++)
  {
    if (it->second >= 0)
    {
      curr_normal_rank = it->second - rank_mean;
      for (int i = 0; i < num_movies_seen_user; i++)
      {
        curr_feature = (_movies_map.find (it->first))->second;
        for (unsigned int j = 0; j < curr_feature.size (); j++)
        {
          rcmnd_vec[j] = rcmnd_vec[j] + (curr_normal_rank * curr_feature[j]);
        }
      }
    }
  }
  return rcmnd_vec;
}

/**
 * a function that calculates the movie with highest score based on
 * movie features
 * @param ranks user ranking to use for algorithm
 * @return shared pointer to movie in system
 */
sp_movie RecommenderSystem::recommend_by_content (const RSUser &user)
{
  //we going throught the rank map and check if a movie isn't appeared in
  // the user movies, if it isn't we caclulate it similarity and compare it
  // to the max value and if it larger than that we update the max movie
  double curr_similarity;
  sp_movie max_similarity_movie;
  rank_map user_rank_map = user.get_ranks ();
  double max_similarity = DEFAULT_VALUE_COMPARE;
  std::vector<double> recommended_vector = find_user_recommended_vec (user);
  std::map<sp_movie, std::vector<double> >::iterator it;
  for (it = _movies_map.begin (); it != _movies_map.end (); it++)
  {
    if (user_rank_map.find (it->first) == user_rank_map.end () ||
        (user_rank_map.find (it->first))->second == -1)
    {
      curr_similarity = calculate_similarity (recommended_vector, it->second);
      if (max_similarity < curr_similarity)
      {
        max_similarity = curr_similarity;
        max_similarity_movie = it->first;
      }
    }
  }
  return max_similarity_movie;
}

double
RecommenderSystem::calculate_similarity (const std::vector<double> &rec_vec,
                                         const std::vector<double> &features)
{
  double product = vector_multiply (rec_vec, features);
  double norm1 = get_norm (rec_vec, rec_vec.size ());
  double norm2 = get_norm (features, features.size ());
  double similarity = product / (norm1 * norm2);
  return similarity;
}

double RecommenderSystem::get_norm (const std::vector<double> &vec, int n)
const
{
  double sum = 0;
  for (int i = 0; i < n; ++i)
  {
    sum += vec[i] * vec[i];
  }
  return sqrt (sum);
}

double
RecommenderSystem::vector_multiply (const std::vector<double> &vec1, const
std::vector<double> &vec2)
{
  int vec_size = std::min (vec1.size (), vec2.size ());
  double product = 0;
  for (int i = 0; i < vec_size; i++)
  {
    product += vec1[i] * vec2[i];
  }
  return product;
}

double RecommenderSystem::get_predict_helper (const RSUser &user,
                                              std::map<double, sp_movie>
                                              &similarities, int k)
{
  rank_map user_rank_map = user.get_ranks ();
  double up_value = 0;
  double down_value = 0;
  double curr_simil_value, curr_rank;
  for (int i = 0; i < k; i++)
  {
    auto it = similarities.begin ();
    std::advance (it, i);
    //return it to the original value
    curr_simil_value = -(it->first);
    curr_rank = user_rank_map[it->second];
    up_value += curr_simil_value * curr_rank;
    down_value += curr_simil_value;
  }
  return up_value / down_value;
}

double RecommenderSystem::predict_movie_score (const RSUser &user, const
sp_movie &movie,
                                               int k)
{
  //the movie that we want to reccomend to the user(she didn't see it)
  std::vector<double> movie_features = _movies_map.find (movie)->second;
  //the movie the user already has seen
  std::vector<double> ranked_movie_features;
  //save the movie and it's similarity
  std::map<double, sp_movie> all_similarities;
  double curr_similarity;
  rank_map user_rank_map = user.get_ranks ();
  rank_map::iterator it;
  for (it = user_rank_map.begin (); it != user_rank_map.end (); it++)
  {
    if (it->second != -1)
    {
      //find the ranked movie in the movies map and returns it features
      ranked_movie_features = _movies_map.find (it->first)->second;
      curr_similarity = calculate_similarity (ranked_movie_features,
                                              movie_features);
      //the map order is increasing so when i put - it reverse its order
      all_similarities[-curr_similarity] = it->first;
    }
  }
  return get_predict_helper (user, all_similarities, k);
}

sp_movie RecommenderSystem::recommend_by_cf (const RSUser &user, int k)
{
  rank_map user_rank_map = user.get_ranks ();
  sp_movie best_match_movie;
  double max_matching_value = DEFAULT_VALUE_COMPARE;
  double curr_predict_rank;
  std::map<sp_movie, std::vector<double>>::iterator it;
  for (it = _movies_map.begin (); it != _movies_map.end (); it++)
  {
    if (user_rank_map.find (it->first) == user_rank_map.end ())
    {
      curr_predict_rank = predict_movie_score (user, it->first, k);
      if (max_matching_value < curr_predict_rank)
      {
        best_match_movie = it->first;
        max_matching_value = curr_predict_rank;
      }
    }
  }
  return best_match_movie;
}

sp_movie RecommenderSystem::get_movie (const std::string &name, int year)
const
{
  sp_movie movie = std::make_shared<Movie> (name, year);
  auto it = _movies_map.find (movie);
  if (it != _movies_map.end ())
  {
    return it->first;
  }
  return nullptr;
}

std::ostream &operator<< (std::ostream &s, const RecommenderSystem
&rs)
{
  std::map<sp_movie, std::vector<double> >::const_iterator it;
  for (it = rs._movies_map.begin (); it != rs._movies_map.end (); it++)
  {
    s << *(it->first);
  }
  return s;
}


