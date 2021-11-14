#!/bin/bash
make
sudo valgrind --leak-check=full ./server -i 0.0.0.0 -p 8080
