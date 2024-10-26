#!/bin/bash

CURRENT_DIR=$(pwd)
# Directorios donde se encuentran los experimentos
DIR_EXP1="$CURRENT_DIR/experimentos/exp1"
DIR_EXP2="$CURRENT_DIR/experimentos/exp2"
DIR_RESULTADOS="$CURRENT_DIR/experimentos/resultados"
GRAFICADOR_DIR="$CURRENT_DIR/experimentos"

# Paso 1: Eliminar archivos "tester"
echo "Eliminando archivos tester..."

if [ -f "$DIR_EXP2/stressTester.txt" ]; then
    rm "$DIR_EXP2/stressTester.txt"
    echo "Archivo stressTester.txt eliminado."
else
    echo "Archivo stressTester.txt no encontrado."
fi

if [ -f "$DIR_EXP1/promedioTester.txt" ]; then
    rm "$DIR_EXP1/promedioTester.txt"
    echo "Archivo promedioTester.txt eliminado."
else
    echo "Archivo promedioTester.txt no encontrado."
fi

# Paso 2: Eliminar ejecutables de los experimentos
echo "Eliminando ejecutables de los experimentos..."

if [ -f "$DIR_EXP1/exp" ]; then
    rm "$DIR_EXP1/exp"
    echo "Ejecutable del experimento 1 eliminado."
else
    echo "Ejecutable del experimento 1 no encontrado."
fi

if [ -f "$DIR_EXP2/exp2" ]; then
    rm "$DIR_EXP2/exp2"
    echo "Ejecutable del experimento 2 eliminado."
else
    echo "Ejecutable del experimento 2 no encontrado."
fi

# Paso 3: Eliminar gráficos generados
echo "Eliminando gráficos generados..."

if [ -f "$GRAFICADOR_DIR/experimento1.jpg" ]; then
    rm "$GRAFICADOR_DIR/experimento1.jpg"
    echo "Archivo experimento1.jpg eliminado."
else
    echo "Archivo experimento1.jpg no encontrado."
fi

if [ -f "$GRAFICADOR_DIR/experimento2.jpg" ]; then
    rm "$GRAFICADOR_DIR/experimento2.jpg"
    echo "Archivo experimento2.jpg eliminado."
else
    echo "Archivo experimento2.jpg no encontrado."
fi

if [ -f "$DIR_RESULTADOS/resultadoExperimento1.txt" ]; then
    rm "$DIR_RESULTADOS/resultadoExperimento1.txt"
    echo "Archivo resultadoExperimento1.txt eliminado."
else
    echo "Archivo resultadoExperimento1.txt no encontrado."
fi

if [ -f "$DIR_RESULTADOS/resultadoExperimento2.txt" ]; then
    rm "$DIR_RESULTADOS/resultadoExperimento2.txt"
    echo "Archivo resultadoExperimento2.txt eliminado."
else
    echo "Archivo resultadoExperimento2.txt no encontrado."
fi

echo "Todos los archivos generados han sido eliminados (si existían)."
