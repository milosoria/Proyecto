#!/bin/bash
make
valgrind --leak-check=full ./client -i 0.0.0.0 -p 8080
# << EOF
# camilo
# 0
# 0
# 9
# 0
# EOF
