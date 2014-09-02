composer
========
An HMM-supported automatic composer, which produces melodies that somehow resemble the given training songs.
composer.cpp: main module that composes, takes several lines of single integer as its input, and a single line of integers seperated by spaces as its output. n for the first line, indicating the total number of scores, followed by n integers for the next n lines, i.e., 0 for a rest, 1 for 'do', 2 for 're', etc.
play.py: a script that translates and plays (via cxq's server) the output of composer.
scores: a short piece of canon, as a testing training set
mine: a piece of melody I generated, could be played by play.py
