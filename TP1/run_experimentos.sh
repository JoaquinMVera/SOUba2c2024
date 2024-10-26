#!/bin/bash

CURRENT_DIR=$(pwd)
# Directorios donde se encuentran los experimentos
DIR_EXP1="$CURRENT_DIR/experimentos/exp1"
DIR_EXP2="$CURRENT_DIR/experimentos/exp2"
DIR_SCRIPTS="$CURRENT_DIR/scripts"
GRAFICADOR_DIR="$CURRENT_DIR/experimentos"

# Paso 1: Generar archivos "tester" con AWK
echo "Generando archivos tester..."

cd "$DIR_SCRIPTS" || { echo "Error: No se puede acceder al directorio $DIR_SCRIPTS"; exit 1; }
awk -f contar-palabras-multifile.awk ../data/exp2/data1.txt ../data/exp2/data2.txt ../data/exp2/data3.txt > "$DIR_EXP2/stressTester.txt"
awk -f contar-palabras-multifile.awk ../data/exp1/data1_processed.txt ../data/exp1/data2_processed.txt ../data/exp1/data3_processed.txt > "$DIR_EXP1/promedioTester.txt"
echo "Archivos tester generados."

# Paso 2: Compilar los experimentos

echo "Compilando el experimento 1..."
cd "$DIR_EXP1" || { echo "Error: No se puede acceder al directorio $DIR_EXP1"; exit 1; }
g++ ../../src/HashMapConcurrente.cpp ../../src/CargarArchivos.cpp experimento.cpp -o exp -std=c++17 -pthread
if [ $? -ne 0 ]; then
    echo "Error en la compilación del experimento 1."
    exit 1
fi
echo "Compilación del experimento 1 completada."

echo "Compilando el experimento 2..."
cd "$DIR_EXP2" || { echo "Error: No se puede acceder al directorio $DIR_EXP2"; exit 1; }
g++ ../../src/HashMapConcurrente.cpp ../../src/CargarArchivos.cpp experimento2.cpp -o exp2 -std=c++17 -pthread
if [ $? -ne 0 ]; then
    echo "Error en la compilación del experimento 2."
    exit 1
fi
echo "Compilación del experimento 2 completada."

# Paso 3: Ejecutar los experimentos

echo "Ejecutando el experimento 1..."
cd "$DIR_EXP1" || { echo "Error: No se puede acceder al directorio $DIR_EXP1"; exit 1; }
./exp
if [ $? -ne 0 ]; then
    echo "Error al ejecutar el experimento 1."
    exit 1
fi
echo "Experimento 1 completado."

echo "Ejecutando el experimento 2..."
cd "$DIR_EXP2" || { echo "Error: No se puede acceder al directorio $DIR_EXP2"; exit 1; }
./exp2
if [ $? -ne 0 ]; then
    echo "Error al ejecutar el experimento 2."
    exit 1
fi
echo "Experimento 2 completado."

# Paso 4: Generar los gráficos
cd "$GRAFICADOR_DIR" || { echo "Error: No se puede acceder al directorio $GRAFICADOR_DIR"; exit 1; }
echo "Generando gráficos con python..."
python3 ./graficador.py
if [ $? -ne 0 ]; then
    echo "Error al generar los gráficos."
    exit 1
fi
echo "Gráficos generados exitosamente."

echo "Todo el proceso ha sido completado."
