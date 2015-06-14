#!/bin/bash
#
# Script to generate basis.mmod for cpp version

# Note that basis.morpho needs to be compiled with morpho.jar, not morpho2
java -jar ../morphodist/bin/morpho.jar -c  basis.morpho
