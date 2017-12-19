#ifndef CONSTANTS_H_
#define CONSTANTS_H_

const int width = 500;
const int height = width;
const double aspectRatio = width / height;
const int cellWidth = width / 100;
const int cellHeight = cellWidth * aspectRatio;
const int cellHcount = width / cellWidth;
const int cellVcount = cellHcount * aspectRatio;
const int cellCount = cellHcount * cellVcount;
const int maxBots = 400; 
const int FPS = 60;
const int spawnRadius = 10; // cells
const int spawnDelay = 0.1; // seconds
const int moveRadius = 2;

const int scanSize = cellHcount / 20; // in cells
const int scanRegionWidth = cellWidth * scanSize; // pixels per scan region
const int scanRegionHeight = scanRegionWidth; // pixels per scan region
const int scanRegionHcount = width / scanRegionWidth;
const int scanRegionVcount = height / scanRegionHeight;

const bool enableNoise = true;
const int noiseDensity = 300; // %

#endif