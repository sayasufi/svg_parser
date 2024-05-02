import logging
import re

import numpy as np
from matplotlib import pyplot as plt, patches
from svg.path import parse_path
from svg.path.path import Line, QuadraticBezier, CubicBezier, Arc, Close


class Path:
    def __init__(self, element, count):
        self.element = element

        # Тег отрисовки

        # Первое значение (0.14412113) - масштабирование по оси X
        # Второе значение (0.17010015) - сдвиг по оси Y
        # Третье значение (-0.15272909) - сдвиг по оси X
        # Четвертое значение (0.16888896) - масштабирование по оси Y
        # Пятое и шестое значения (16.178206, 18.279143) - смещение элемента по осям X и Y соответственно

        d = element.get("d")
        self.segments = parse_path(d)

        # Тег стилизации
        style = element.get("style")
        pattern = re.compile(r"(?:^|;)([^:]+):([^;]+)")
        self.style_dict = dict(re.findall(pattern, style))

        # Тег преобразования
        transform = element.get("transform")
        numbers = [
            float(num) for num in transform.split("(")[1].split(")")[0].split(",")
        ]
        temp_transform_matrix = np.array(
            [
                [numbers[0], numbers[2], numbers[4]],
                [numbers[1], numbers[3], numbers[5]],
                [0, 0, 1],
            ]
        )
        rotation_matrix = np.array([[-1, 0, 0], [0, -1, 0], [0, 0, 1]])
        self.transform_matrix = np.dot(temp_transform_matrix, rotation_matrix)

        self.count = count

    def start(self):
        # Создание графика
        fig, ax = plt.subplots()
        ax.set_aspect("equal", adjustable="datalim")

        for segment in self.segments:
            if isinstance(segment, (Line, Close)):
                start = np.array([segment.start.real, segment.start.imag, 1])
                end = np.array([segment.end.real, segment.end.imag, 1])
                transformed_start = np.dot(self.transform_matrix, start)
                transformed_end = np.dot(self.transform_matrix, end)
                plt.plot(
                    [transformed_start[0], transformed_end[0]],
                    [transformed_start[1], transformed_end[1]],
                    color=self.style_dict["stroke"],
                    linewidth=self.style_dict["stroke-width"],
                )
            elif isinstance(segment, QuadraticBezier):
                plt.plot(
                    [segment.start.real, segment.control.real, segment.end.real],
                    [segment.start.imag, segment.control.imag, segment.end.imag],
                )
            elif isinstance(segment, CubicBezier):
                plt.plot(
                    [
                        segment.start.real,
                        segment.control1.real,
                        segment.control2.real,
                        segment.end.real,
                    ],
                    [
                        segment.start.imag,
                        segment.control1.imag,
                        segment.control2.imag,
                        segment.end.imag,
                    ],
                )
            elif isinstance(segment, Arc):
                arc = segment

                def calculate_arc_center(arc):
                    radius = abs(arc.radius.real) + abs(arc.radius.imag)  # Радиус дуги
                    start_angle = np.angle(arc.start - arc.center)  # Начальный угол
                    end_angle = np.angle(arc.end - arc.center)  # Конечный угол
                    center = (arc.center.real, arc.center.imag)  # Центр дуги

                    return center, start_angle, end_angle, radius

                center, start_angle, end_angle, radius = calculate_arc_center(arc)

                new_start_angle = start_angle

                new_end_angle = end_angle

                print(center, radius, np.degrees(new_start_angle), np.degrees(new_end_angle))
                arc1 = patches.Arc(
                    center,
                    radius,
                    radius,
                    angle=0,
                    theta1=np.degrees(new_start_angle),
                    theta2=np.degrees(new_end_angle),
                )
                ax.add_patch(arc1)
                plt.gca().set_aspect("equal", adjustable="box")

        plt.grid(True)
        plt.xlim(-10, 10)
        plt.ylim(-10, 10)
        plt.axis("equal")
        plt.show()


class Ellipse:
    def __init__(self, element, count):
        # Обработка данных из тега ellipse
        self.cx = float(element.get("cx"))
        self.cy = float(element.get("cy"))
        self.rx = float(element.get("rx"))
        self.ry = float(element.get("ry"))

        # Тег стилизации
        style = element.get("style")
        pattern = re.compile(r"(?:^|;)([^:]+):([^;]+)")
        self.style_dict = dict(re.findall(pattern, style))

        self.count = count

    def start(self):
        logging.info(
            f"\tgapi_hdl gapi_hdl_{self.count};\n"
            f"\tgapi_hdl_{self.count}.color = {self.style_dict['stroke']};\n"
            f"\tgapi_hdl_{self.count}.line_width = {self.style_dict['stroke-width']};"
        )

        logging.info(
            f"\tvoid gapi_ellipse(&gapi_hdl_{self.count}, {self.cx}, {self.cy}, {self.rx}, {self.ry});\n"
        )
