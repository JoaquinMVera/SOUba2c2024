import matplotlib.pyplot as plt

def graficar_resultados(archivo, salida):
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

    if(salida == 'experimento1.jpg'):
        plt.plot(threads, tiempos, marker='x', linestyle='-', color='r')
    else:
        plt.plot(threads, tiempos, marker='x', linestyle='-', color='b')
    

    # Etiquetas y titulos para esto
    plt.xlabel('Cantidad de Threads')
    plt.ylabel('Tiempo (ms)')

    if(salida == 'experimento1.jpg'):
        plt.title('Tiempo de calculo del promedio paralelo para distintos threads \n')
    else:
         plt.title('Tiempo de insercion para distintos threads \n')

    
    # Devolvemos el grafico
    plt.savefig(salida, format='jpg', dpi=300)

# Ejemplo de uso
resultadoExperimento1 = './resultados/resultadoExperimento1.txt'
resultadoExperimento2 = './resultados/resultadoExperimento2.txt'
graficar_resultados(resultadoExperimento1, 'experimento1.jpg')
graficar_resultados(resultadoExperimento2, 'experimento2.jpg')
