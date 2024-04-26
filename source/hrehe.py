from svg.path import parse_path
from svg.path.path import Path, Line, QuadraticBezier, CubicBezier, Arc, Close
import matplotlib.pyplot as plt

# Функция для разбора тега <path> и отрисовки его с помощью простых фигур
def draw_svg_path(svg_file):
    with open(svg_file, 'r') as file:
        data = file.read()

    path_data = "m 15,15.647321 -25.323511,25.748791 -9.637351,-34.8051986 -9.637352,-34.8051994 34.9608621,9.056409 34.9608629,9.056408 z"
    path = parse_path(path_data)

    for segment in path:
        print(segment)
        if isinstance(segment, Line):
            plt.plot([segment.start.real, segment.end.real], [segment.start.imag, segment.end.imag])
        elif isinstance(segment, QuadraticBezier):
            plt.plot([segment.start.real, segment.control.real, segment.end.real],
                    [segment.start.imag, segment.control.imag, segment.end.imag])
        elif isinstance(segment, CubicBezier):
            plt.plot([segment.start.real, segment.control1.real, segment.control2.real, segment.end.real],
                    [segment.start.imag, segment.control1.imag, segment.control2.imag, segment.end.imag])
        elif isinstance(segment, Arc):
            # Обработка дуги
            pass
        elif isinstance(segment, Close):
            plt.plot([segment.start.real, segment.end.real], [segment.start.imag, segment.end.imag])
        # Добавьте обработку других типов сегментов по аналогии

    plt.axis('equal')
    plt.show()

# Пример использования
draw_svg_path('files/Маршрут/ППМ.svg')