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

                def calculate_arc(arc):
                    radius = (abs(arc.radius.real) + abs(arc.radius.imag)) / 2  # Радиус дуги
                    start_angle = np.angle(arc.start - arc.center)  # Начальный угол
                    end_angle = np.angle(arc.end - arc.center)  # Конечный угол
                    center = np.array([arc.center.real, arc.center.imag, 1])  # Центр дуги

                    start_x = center[0] + radius * np.cos(start_angle)
                    start_y = center[1] + radius * np.sin(start_angle)

                    end_x = center[0] + radius * np.cos(end_angle)
                    end_y = center[1] + radius * np.sin(end_angle)

                    return center, radius, np.array([start_x, start_y, 1]), np.array([end_x, end_y, 1])

                def calculate_arc_angles(center, start_point, end_point):
                    start_angle = np.arctan2(start_point[1] - center[1], start_point[0] - center[0])
                    end_angle = np.arctan2(end_point[1] - center[1], end_point[0] - center[0])

                    return start_angle, end_angle

                center, radius, start_point, end_point = calculate_arc(arc)
                new_start_point = np.dot(self.transform_matrix, start_point)
                new_end_point = np.dot(self.transform_matrix, end_point)
                new_start_angle, new_end_angle = calculate_arc_angles(center, new_start_point, new_end_point)

                # print(center, radius, np.degrees(new_start_angle), np.degrees(new_end_angle))

                arc1 = patches.Arc(
                    (center[0], center[1]),
                    radius * 2,
                    radius * 2,
                    angle=0,
                    theta1=np.degrees(new_start_angle),
                    theta2=np.degrees(new_end_angle),
                )
                ax.add_patch(arc1)

        plt.grid(True)
        plt.xlim(-10, 10)
        plt.ylim(-10, 10)
        plt.gca().set_aspect("equal", adjustable="box")
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
