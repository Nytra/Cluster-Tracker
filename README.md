# Cluster-Tracker
It's a graphics / computer vision demo of some clusters of coloured pixels that get tracked by a custom algorithm that works by iterating over the raw pixel array and finding areas where the density of same-colour pixels is high. 

Requires SFML.

---

![gif](cluster_tracker.gif)

## How it works

The program stores pixel data in a one-dimensional array. Some bots (the red, green or blue pixels) are scattered about. They are programmed to move toward other pixels of the same colour (There are also other behaviour types that can be chosen by changing a const enum value in the source code)

The algorithm scans the screen in a grid pattern. The grid squares will fit about 16 pixels or cells inside. We calculate the pixel or cell density of this grid square. If it is above a certain threshold, we determine that a cluster is present.

There's also some code to ensure that the same cluster is not marked more than once. The sensitivity of the algorithm could be improved, as you can see in the above GIF, some smaller clusters don't get found.

I've also added noise in the form of yellow pixels, just to test how good the algorithm is.
