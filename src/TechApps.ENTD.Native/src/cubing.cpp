#include "cubing.h"

vector<vector<float>> getSurface1(vector<vector<vector<float>>> cube)
{
  auto mid = ((cube.size()-1)/2);
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0 ; i < cube.size(); i++) {
    for (size_t j = 0 ; j < cube[i].size(); j++) {
      surface[i][j] = cube[i][j][mid];
    }
  }
  return surface;
}

vector<vector<float>> getSurface2(vector<vector<vector<float>>> cube)
{
  auto mid = ((cube.size()-1)/2);
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0 ; i < cube.size(); i++) {
    for (size_t j = 0 ; j < cube[i].size(); j++) {
      surface[i][j] = cube[i][mid][j];
    }
  }
  return surface;
}

vector<vector<float>> getSurface3(vector<vector<vector<float>>> cube)
{
  auto mid = ((cube.size()-1)/2);
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0 ; i < cube.size(); i++) {
    for (size_t j = 0 ; j < cube[i].size(); j++) {
      surface[i][j] = cube[mid][i][j];
    }
  }
  return surface;
}

vector<vector<float>> getSurface4(vector<vector<vector<float>>> cube)
{
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0 ; i < cube.size(); i++) {
    for (size_t j = 0 ; j < cube[i].size(); j++) {
      surface[i][j] = cube[i][i][j];
    }
  }
  return surface;
}

vector<vector<float>> getSurface5(vector<vector<vector<float>>> cube)
{
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0, q = cube.size() - 1; i < cube.size(); i++, q--) {
    for (size_t j = 0 ; j < cube[i].size(); j++) {
      surface[i][j] = cube[i][q][j];
    }
  }
  return surface;
}

vector<vector<float>> getSurface6(vector<vector<vector<float>>> cube)
{
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0 ; i < cube.size(); i++)
  {
    for (size_t j = 0 ; j < cube[i].size(); j++)
    {
      surface[i][j] = cube[i][j][i];
    }
  }
  return surface;
}

vector<vector<float>> getSurface7(vector<vector<vector<float>>> cube)
{
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0, r = cube.size() -1; i < cube.size(); i++, r--) {
    for (size_t j = 0 ; j < cube[i].size(); j++) {
      surface[i][j] = cube[i][j][r];
    }
  }
  return surface;
}

vector<vector<float>> getSurface8(vector<vector<vector<float>>> cube)
{
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0 ; i < cube.size(); i++) {
    for (size_t j = 0 ; j < cube[i].size(); j++) {
      surface[i][j] = cube[j][i][i];
    }
  }
  return surface;
}

vector<vector<float>> getSurface9(vector<vector<vector<float>>> cube)
{
  vector<vector<float>> surface(cube.size(), vector<float>(cube.size()));
  for(size_t i = 0, q = cube.size() - 1; i < cube.size(); i++, q--) {
    for (size_t j = 0 ; j < cube[i].size(); j++) {
      surface[i][j] = cube[j][q][i];
    }
  }
  return surface;
}
