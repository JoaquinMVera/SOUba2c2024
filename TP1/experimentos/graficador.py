import matplotlib.pyplot as plt

def graficar_resultados(archivo):
    threads = []
    tiempos = []
    
    # Leemos el archivo y hacemos toda la logica de separar el csv. Quedo medio feo
    # pero creemos que queda mejor que los logs digan cosas como el experimento, y algo mas de flavour text
    with open(archivo, 'r') as file:
        for linea in file:
            partes = linea.strip().split(',')
            if len(partes) == 2:
                thread_info = partes[0].split(':')[-1].strip()
                tiempo = partes[1].strip()
                threads.append(int(thread_info))
                tiempos.append(float(tiempo))
    
    # Aca ya graficamos!
    plt.figure(figsize=(10, 6))
    plt.plot(threads, tiempos, marker='x', linestyle='-', color='b')

    # Etiquetas y titulos para esto
    plt.xlabel('Cantidad de Threads')
    plt.ylabel('Tiempo (ms)')
    plt.title('Tiempo de insercion para distintos threads \n')
    
    # Devolvemos el grafico
    plt.show()

# Ejemplo de uso
resultadoExperimento1 = './resultados/resultadoExperimento1.txt'
resultadoExperimento2 = './resultados/resultadoExperimento2.txt'
graficar_resultados(resultadoExperimento1)
graficar_resultados(resultadoExperimento2)
