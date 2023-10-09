import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

def plot_points_with_dual_aura(matrix_size, grid_size, sensor_points, sensing_range):
    # Criar uma matriz preenchida com zeros.
    matrix = np.zeros((matrix_size, matrix_size))

    # Colocar pontos de sensores na matriz.
    for x, y in sensor_points:
        # Arredondar as coordenadas para os índices inteiros da matriz.
        x = int(round(x))
        y = int(round(y))
        # Garantir que as coordenadas estejam dentro dos limites da matriz.
        x = max(0, min(x, matrix_size-1))
        y = max(0, min(y, matrix_size-1))
        matrix[y, x] = 1

    # Criar uma grade na plotagem para facilitar a visualização.
    grid_interval = matrix_size // grid_size
    x_ticks = np.arange(0, matrix_size+1, grid_interval)
    y_ticks = np.arange(0, matrix_size+1, grid_interval)

    # Plotar a matriz com os pontos dos sensores.
    #plt.imshow(matrix, cmap='Blues', origin='lower')


    cont = 0
    for psov in demand_points:
        x = (cont % 5) * 10 + 5
        y = int(cont / 5) * 10 + 5
        print(y)
        match psov:
            case 0:
                inner_circle = plt.Circle((x, y), 1, color='black', alpha=1, fill=False, linewidth=1.3)
                plt.gca().add_patch(inner_circle)
            case 1:
                inner_circle = plt.Circle((x, y), 1, color='lightblue', alpha=1, linewidth=1.3)
                plt.gca().add_patch(inner_circle)
                inner_circle.set_edgecolor('blue')  # Definindo borda preta para o caso 1
            case 2:
                inner_circle = plt.Circle((x, y), 1, color='blue', alpha=1, linewidth=1.3)
                plt.gca().add_patch(inner_circle)
                inner_circle.set_edgecolor('black')  # Definindo borda preta para o caso 2
        cont += 1


        
    # Plotar as duas auras em volta de cada ponto de sensor.
    for x, y in sensor_points:
        # Criar a aura menor representando a certeza de cobertura.
        inner_circle = plt.Circle((x, y), sensing_range, color='blue', alpha=0.8, fill=False, linewidth=0.7, zorder=2)
        plt.gca().add_patch(inner_circle)

        # Criar o triângulo representando o sensor.
        triangle = Polygon([(x, y+1), (x-1, y-1), (x+1, y-1)], closed=True, facecolor='yellow', edgecolor='black', zorder=2)
        plt.gca().add_patch(triangle)


    plt.axhspan(-7, -0.2, facecolor='white', alpha=1.0, zorder=2)
    plt.axhspan(50, 57, facecolor='white', alpha=1.0, zorder=2)
    plt.axvspan(-7, 0, facecolor='white', alpha=1.0, zorder=2)
    plt.axvspan(50.2, 57, facecolor='white', alpha=1.0, zorder=2)

    plt.xticks(x_ticks)
    plt.yticks(y_ticks)
    plt.grid(True, which='both', linewidth=0.8, color='black', linestyle='-', zorder=3)
    plt.axis('image')
    plt.show()

if __name__ == "__main__":
    matrix_size = 50
    grid_size = 5
    # Exemplo com coordenadas float.
    sensor_points = [(10.1, 6.4), (29.8, 6.9), (14.4, 40.3), (25.6, 40.3), (5.9, 40), (29.9, 25.2), (40, 43.9), (39.7, 35.1), (29.9, 16.4), (44.1, 19.9), (10, 24.4), (10, 13.5), (41.5, 5.5)]
    demand_points = [0, 1, 2, 0, 0, 1, 0, 2, 2, 1, 2, 1, 0]
    sensing_range = 10


    plot_points_with_dual_aura(matrix_size, grid_size, sensor_points, sensing_range)
