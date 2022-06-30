#include "RSUsersLoader.h"
#include <unordered_map>
#define WRONG_NUMBER "wrong number"
#define NA "NA"
#define NA_NEW_LINE "NA\n"
#define NA_R "NA\r"
#define HAVENT_BEEN_RANK -1
#define LOWER_BOUND 1
#define HIGHER_BOUND 10
#define SPACE " "
#define PROBLEM_OPEN_THE_FILE "ERROR: problem opening the file!"
#define YEAR_DIGIT_NUMBER 5
#define MAKAF "-"

/*
 * read and make the rank map for every users line we get and update the
 * rank map
 */
void read_and_push_user_rankmap (std::vector<sp_movie>
                                 &movies, string line,
                                 rank_map &user_rank_map)
{
  std::size_t e_rank;
  string curr_rank;
  for (const auto &i: movies)
  {
    e_rank = line.find (SPACE);
    curr_rank = line.substr (0, e_rank);
    if (curr_rank == NA || curr_rank == NA_R || curr_rank == NA_NEW_LINE)
    {
      user_rank_map[i] = HAVENT_BEEN_RANK;
    }
    else if (std::stod (curr_rank) >= LOWER_BOUND && std::stod (curr_rank) <=
                                                     HIGHER_BOUND)
    {
      user_rank_map[i] = std::stod (curr_rank);
    }
    else
    {
      throw std::runtime_error (WRONG_NUMBER);
    }
    line.erase (0, e_rank + 1);
  }
}

std::vector<RSUser> RSUsersLoader::create_users_from_file (const
                                                           std::string &
users_file_path, unique_ptr<RecommenderSystem> rs) noexcept (false)
{
  std::ifstream is;
  is.open (users_file_path, std::ios::in);
  if (!is.is_open ())
  {
    throw std::runtime_error (PROBLEM_OPEN_THE_FILE);
  }
  std::shared_ptr<RecommenderSystem> shared_rs = std::move (rs);
  std::vector<RSUser> users; //return users
  std::vector<sp_movie> movies;
  string line, curr_rank, movie_name, movie_year;
  std::getline (is, line);
  sp_movie movie_ptr;
  std::vector<double> curr_users_rank;
  //ADD THE MOVIES
  while (line.find (MAKAF) != std::string::npos)
  {
    movie_name = line.substr (0, line.find (MAKAF));
    line.erase (0, line.find (MAKAF) + 1);
    movie_year = line.substr (0, 4);
    movie_ptr = std::make_shared<Movie> (movie_name, std::stoi (movie_year));
    line.erase (0, YEAR_DIGIT_NUMBER);
    movies.push_back (movie_ptr);
  }
  while (std::getline (is, line)) //adding all the user
  {
    string user_name = line.substr (0, line.find (SPACE));
    line.erase (0, line.find (SPACE) + 1);
    rank_map user_rank_map (0, sp_movie_hash, sp_movie_equal);
    read_and_push_user_rankmap (movies, line, user_rank_map);
    RSUser curr_user (user_name, user_rank_map, shared_rs);
    users.push_back (curr_user);
  }
  is.close ();
  return users;
}

