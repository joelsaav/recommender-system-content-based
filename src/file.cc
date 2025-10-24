#include "../include/recommender-system.h"

RecommenderSystem::RecommenderSystem(const std::string& matrixFile, int metric,
                                     int numNeighbors, int predictionType)
    : metric_(metric),
      numNeighbors_(numNeighbors),
      predictionType_(predictionType) {
  std::ifstream file(matrixFile);
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << matrixFile << std::endl;
    exit(1);
  }
  std::string line;
  file >> minRating_ >> maxRating_;
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    std::vector<std::string> row;
    std::string value;
    std::istringstream ss(line);
    while (ss >> value) {
      row.push_back(value);
    }
    userItemMatrix_.push_back(row);
  }
  file.close();

  similaritiesPerUser_.resize(userItemMatrix_.size());
  neighborsPerUser_.reserve(userItemMatrix_.size());
  userItemMatrixCopy_ = userItemMatrix_;

  CalculateUserMean();
  CalculateSimilarities();
  ChooseNeighbors();
  CountUnknownRatingsPerUser();
  CountUnknownRatingsPerItem();
}

const std::vector<std::vector<std::string>>& RecommenderSystem::userItemMatrix()
    const {
  return userItemMatrix_;
}

const std::vector<std::vector<std::string>>&
RecommenderSystem::userItemMatrixCopy() const {
  return userItemMatrixCopy_;
}

const std::vector<std::vector<double>>& RecommenderSystem::similaritiesPerUser()
    const {
  return similaritiesPerUser_;
}

const std::vector<std::vector<int>>& RecommenderSystem::neighborsPerUser()
    const {
  return neighborsPerUser_;
}

void RecommenderSystem::Recommend() {
  std::cout << "\n╔════════════════════════════════════════╗\n";
  std::cout << "║              PREDICTIONS               ║\n";
  std::cout << "╚════════════════════════════════════════╝\n";
  for (const std::pair<int, int>& p : unknownRatingsPerUser_) {
    for (const std::pair<int, int>& q : unknownRatingsPerItem_) {
      if (userItemMatrix_[p.first][q.first] != "-") {
        continue;
      }
      double predictedScore = PredictRating(p.first, q.first);
      std::string predictedStr = std::to_string(predictedScore);
      userItemMatrix_[p.first][q.first] = predictedStr;
      std::cout << "Predicted score for user " << p.first << " and item "
                << q.first << ": " << predictedScore << std::endl;
    }
  }
}

void RecommenderSystem::CalculateUserMean() {
  for (size_t i = 0; i < userItemMatrix_.size(); ++i) {
    double sum = 0.0;
    int count = 0;
    for (const std::string& rating : userItemMatrix_[i]) {
      if (rating == "-") continue;
      double value = std::stod(rating);
      sum += value;
      ++count;
    }
    if (count > 0) {
      userMean_.push_back(sum / count);
    } else {
      userMean_.push_back(0.0);
    }
  }
}

void RecommenderSystem::ChooseNeighbors() {
  for (size_t i = 0; i < similaritiesPerUser_.size(); ++i) {
    std::vector<int> neighbors;
    size_t j = 0;
    bool equal = false;
    while (j < numNeighbors_ && j < similaritiesPerUser_[i].size()) {
      if (j == i) {
        ++j;
        equal = true;
        continue;
      }
      neighbors.push_back(j);
      ++j;
    }

    if (equal && j < similaritiesPerUser_[i].size()) {
      neighbors.push_back(j);
      ++j;
    }

    if (j == similaritiesPerUser_[i].size()) {
      neighborsPerUser_.push_back(neighbors);
      continue;
    }

    while (j < similaritiesPerUser_[i].size()) {
      if (j == i) {
        ++j;
        continue;
      }

      size_t minSimIndex = 0;
      double minSimilarity = similaritiesPerUser_[i][neighbors[0]];
      for (size_t k = 1; k < neighbors.size(); ++k) {
        if (similaritiesPerUser_[i][neighbors[k]] < minSimilarity) {
          minSimilarity = similaritiesPerUser_[i][neighbors[k]];
          minSimIndex = k;
        }
      }

      if (similaritiesPerUser_[i][j] > minSimilarity) {
        neighbors[minSimIndex] = j;
      }
      ++j;
    }
    neighborsPerUser_.push_back(neighbors);
  }

  for (size_t i = 0; i < neighborsPerUser_.size(); ++i) {
    for (size_t j = 0; j < neighborsPerUser_[i].size(); ++j) {
      if (similaritiesPerUser_[i][neighborsPerUser_[i][j]] < 0) {
        neighborsPerUser_[i].erase(neighborsPerUser_[i].begin() + j);
        --j;
      }
    }
  }
}

void RecommenderSystem::CalculateSimilarities() {
  for (size_t i = 0; i < userItemMatrix_.size(); ++i) {
    for (size_t j = 0; j < userItemMatrix_.size(); ++j) {
      if (i == j) {
        similaritiesPerUser_[i].push_back(1.0);
        continue;
      }

      double similarity = 0.0;
      switch (metric_) {
        case 1:
          similarity = PearsonCorrelation(i, j);
          break;
        case 2:
          similarity = CosineDistance(i, j);
          break;
        case 3:
          similarity = EuclideanDistance(i, j);
          break;
        default:
          std::cerr << "Unknown metric type: " << metric_ << std::endl;
          exit(1);
      }
      similaritiesPerUser_[i].push_back(similarity);
    }
  }
}

double RecommenderSystem::PearsonCorrelation(int user1, int user2) const {
  double num = 0.0;
  double denom1 = 0.0;
  double denom2 = 0.0;
  for (size_t i = 0; i < userItemMatrix_[user1].size(); ++i) {
    if (userItemMatrix_[user1][i] == "-" || userItemMatrix_[user2][i] == "-") {
      continue;
    }
    double x = std::stod(userItemMatrix_[user1][i]) - userMean_[user1];
    double y = std::stod(userItemMatrix_[user2][i]) - userMean_[user2];
    num += x * y;
    denom1 += x * x;
    denom2 += y * y;
  }

  if (denom1 == 0.0 || denom2 == 0.0) {
    return 0.0;
  }

  double denom = std::sqrt(denom1) * std::sqrt(denom2);
  return num / denom;
}

double RecommenderSystem::CosineDistance(int user1, int user2) const {
  double num = 0.0;
  double denom1 = 0.0;
  double denom2 = 0.0;
  for (size_t i = 0; i < userItemMatrix_[user1].size(); ++i) {
    if (userItemMatrix_[user1][i] == "-" || userItemMatrix_[user2][i] == "-") {
      continue;
    }
    double x = std::stod(userItemMatrix_[user1][i]);
    double y = std::stod(userItemMatrix_[user2][i]);
    num += x * y;
    denom1 += x * x;
    denom2 += y * y;
  }

  if (denom1 == 0.0 || denom2 == 0.0) {
    return 0.0;
  }

  double denom = std::sqrt(denom1) * std::sqrt(denom2);
  return num / denom;
}

double RecommenderSystem::EuclideanDistance(int user1, int user2) const {
  double sum = 0.0;
  for (size_t i = 0; i < userItemMatrix_[user1].size(); ++i) {
    if (userItemMatrix_[user1][i] == "-" || userItemMatrix_[user2][i] == "-") {
      continue;
    }
    double x = std::stod(userItemMatrix_[user1][i]);
    double y = std::stod(userItemMatrix_[user2][i]);
    sum += (x - y) * (x - y);
  }

  if (sum == 0.0) {
    return 0.0;
  }

  double distance = std::sqrt(sum);
  return 1.0 / distance;
}

double RecommenderSystem::PredictRating(int user, int item) const {
  switch (predictionType_) {
    case 1:
      return PredictRatingSimple(user, item);

    case 2:
      return PredictRatingMeanDifference(user, item);

    default:
      std::cerr << "Unknown prediction type: " << predictionType_ << std::endl;
      exit(1);
  }
}

double RecommenderSystem::PredictRatingSimple(int user, int item) const {
  double num = 0.0;
  double den = 0.0;
  for (size_t i = 0; i < neighborsPerUser_[user].size(); ++i) {
    int neighbor = neighborsPerUser_[user][i];
    if (userItemMatrix_[neighbor][item] != "-") {
      num += similaritiesPerUser_[user][neighbor] *
             std::stod(userItemMatrix_[neighbor][item]);
      den += std::abs(similaritiesPerUser_[user][neighbor]);
    }
  }

  if (den == 0.0) {
    return userMean_[user];
  }

  return num / den;
}

double RecommenderSystem::PredictRatingMeanDifference(int user,
                                                      int item) const {
  double num = 0.0;
  double den = 0.0;
  for (size_t i = 0; i < neighborsPerUser_[user].size(); ++i) {
    int neighbor = neighborsPerUser_[user][i];
    if (userItemMatrix_[neighbor][item] != "-") {
      num += similaritiesPerUser_[user][neighbor] *
             (std::stod(userItemMatrix_[neighbor][item]) - userMean_[neighbor]);
      den += std::abs(similaritiesPerUser_[user][neighbor]);
    }
  }

  if (den == 0.0) {
    return userMean_[user];
  }

  return userMean_[user] + num / den;
}

void RecommenderSystem::CountUnknownRatingsPerUser() {
  for (size_t i{0}; i < userItemMatrix_.size(); ++i) {
    int unknownCount{0};
    for (size_t j{0}; j < userItemMatrix_[i].size(); ++j) {
      if (userItemMatrix_[i][j] == "-") {
        ++unknownCount;
      }
    }
    std::pair<int, int> userUnknowns = std::make_pair(i, unknownCount);
    unknownRatingsPerUser_.push_back(userUnknowns);
  }

  std::sort(unknownRatingsPerUser_.begin(), unknownRatingsPerUser_.end(),
            [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
              return a.second < b.second;
            });
}

void RecommenderSystem::CountUnknownRatingsPerItem() {
  for (size_t i{0}; i < userItemMatrix_[0].size(); ++i) {
    int unknownCount{0};
    for (size_t j{0}; j < userItemMatrix_.size(); ++j) {
      if (userItemMatrix_[j][i] == "-") {
        ++unknownCount;
      }
    }
    std::pair<int, int> itemUnknowns = std::make_pair(i, unknownCount);
    unknownRatingsPerItem_.push_back(itemUnknowns);
  }

  std::sort(unknownRatingsPerItem_.begin(), unknownRatingsPerItem_.end(),
            [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
              return a.second < b.second;
            });
}

std::ostream& operator<<(std::ostream& os, const RecommenderSystem& rs) {
  os << "\n\n╔════════════════════════════════════════╗\n";
  os << "║              INPUT DATA                ║\n";
  os << "╚════════════════════════════════════════╝\n";
  os << "Metric: ";
  switch (rs.metric()) {
    case 1:
      os << "Pearson Correlation\n";
      break;
    case 2:
      os << "Cosine Distance\n";
      break;
    case 3:
      os << "Euclidean Distance\n";
      break;
    default:
      break;
  }
  os << "Prediction Type: ";
  switch (rs.predictionType()) {
    case 1:
      os << "Simple Prediction\n";
      break;
    case 2:
      os << "Mean Difference Prediction\n";
      break;
    default:
      break;
  }
  os << "Number of Neighbors: " << rs.numNeighbors() << "\n";

  os << "\n\n╔════════════════════════════════════════╗\n";
  os << "║            ORIGINAL MATRIX             ║\n";
  os << "╚════════════════════════════════════════╝\n";
  int user = 0;
  for (const std::vector<std::string>& row : rs.userItemMatrixCopy()) {
    os << "User " << user << ": ";
    for (const std::string& rating : row) {
      os << rating << " ";
    }
    os << "\n";
    ++user;
  }

  os << "\n\n╔════════════════════════════════════════╗\n";
  os << "║          SIMILARITIES MATRIX           ║\n";
  os << "╚════════════════════════════════════════╝\n";
  user = 0;
  for (const std::vector<double>& userSimilarities : rs.similaritiesPerUser()) {
    os << "User " << user << ": ";
    for (const double& similarity : userSimilarities) {
      os << std::fixed << std::setprecision(4) << std::setw(8) << similarity
         << " ";
    }
    os << "\n";
    ++user;
  }

  os << "\n╔════════════════════════════════════════╗\n";
  os << "║        CHOSEN NEIGHBORS PER USER       ║\n";
  os << "╚════════════════════════════════════════╝\n";
  user = 0;
  for (const std::vector<int>& neighbors : rs.neighborsPerUser()) {
    os << "User " << user << ": [ ";
    for (size_t i = 0; i < neighbors.size(); ++i) {
      os << neighbors[i];
      if (i < neighbors.size() - 1) os << ", ";
    }
    os << " ]\n";
    ++user;
  }

  os << "\n╔════════════════════════════════════════╗\n";
  os << "║      MATRIX WITH PREDICTED RATINGS     ║\n";
  os << "╚════════════════════════════════════════╝\n";
  user = 0;
  for (const std::vector<std::string>& row : rs.userItemMatrix()) {
    os << "User " << user << ": ";
    for (const std::string& rating : row) {
      os << rating << " ";
    }
    os << "\n";
    ++user;
  }

  os << std::endl;
  return os;
}
