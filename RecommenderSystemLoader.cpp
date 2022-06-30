#include "RecommenderSystemLoader.h"
#define FILE_OPENING_PROBLEM "problem opening the file!"
#define SPACE " "
#define WRONG_NUMBER_PROBLEM "Wrong number! should be between 0 and 10"
#define HIGHER_BOUND 10
#define LOWER_BOUND 1
#define MAKAF "-"
#define SPACE " "
#define NEW_LINE "\n"

/*
 * this function gets the vector features and a string feature and check
 * if it at the right size, if it is so push it into the vector otherwise
 * run time error
 */
void push_and_check_feature (std::vector<double> &features, const string &
feature)
{
  double converted_feature = std::stod (feature);
  if (converted_feature <= HIGHER_BOUND && converted_feature >= LOWER_BOUND)
  {
    features.push_back (converted_feature);
  }
  else
  {
    throw std::runtime_error (WRONG_NUMBER_PROBLEM);
  }
}

unique_ptr<RecommenderSystem>
RecommenderSystemLoader::create_rs_from_movies_file (const std::string &
movies_file_path)
noexcept (false)
{
  std::ifstream is;
  is.open (movies_file_path, std::ios::in);
  if (!is.is_open ())
  {
    throw std::runtime_error (FILE_OPENING_PROBLEM);
  }
  auto rs_pointer = std::make_unique<RecommenderSystem> ();
  string line, name, year, feature;
  while (std::getline (is, line))   //take the first line with the first movie
  {
    std::vector<double> features;
    std::size_t idx_makaf = line.find (MAKAF);   //take the movie name + year
    name = line.substr (0, idx_makaf);
    line.erase (0, idx_makaf + 1);
    year = line.substr (0, line.find (SPACE));
    line.erase (0, line.find (SPACE) + 1);
    while (line.find (SPACE) != std::string::npos)  //take the movie features
    {
      feature = line.substr (0, line.find (SPACE));
      push_and_check_feature (features, feature);
      line.erase (0, line.find (SPACE) + 1);
    }
    if (!line.empty ()) //check the last features
    {
      feature = line.substr (0, line.find (NEW_LINE));
      push_and_check_feature (features, feature);
    }
    rs_pointer->add_movie (name, std::stoi (year), features);
  }
  is.close ();
  return rs_pointer;
}


