import logging
import re

import numpy as np
from svg.path import parse_path
from svg.path.path import Line, QuadraticBezier, CubicBezier, Arc, Close


class Path:
    def __init__(self, element, count):
        self.element = element
        d = element.get("d")
        self.segments = parse_path(d)
        style = element.get("style")
        pattern = re.compile(r"(?:^|;)([^:]+):([^;]+)")
        self.style_dict = dict(re.findall(pattern, style))
        transform = element.get("transform")
        if transform:
            numbers = [float(num) for num in transform.split("(")[1].split(")")[0].split(",")]
            self.transform_matrix = np.array([
                [numbers[0], numbers[2], numbers[4]],
                [numbers[1], numbers[3], numbers[5]],
                [0, 0, 1]
            ])
        else:
            self.transform_matrix = None
        self.count = count

    def apply_transform(self, point):
        if self.transform_matrix is not None:
            return np.dot(self.transform_matrix, np.array([point.real, point.imag, 1]))
        else:
            return np.array([point.real, point.imag, 1])

    def start(self):

        logging.info(
            f"\tgapi_hdl gapi_hdl_{self.count};\n"
            f"\tgapi_hdl_{self.count}.color = {self.style_dict['stroke']};\n"
            f"\tgapi_hdl_{self.count}.line_width = {self.style_dict['stroke-width']};"
        )

        for segment in self.segments:
            if isinstance(segment, (Line, Close, CubicBezier)):
                transformed_start = self.apply_transform(segment.start)
                transformed_end = self.apply_transform(segment.end)
                logging.info(
                    f"\tvoid gapi_line(&gapi_hdl_{self.count}, {transformed_start[0]}, {transformed_start[1]}, {transformed_end[0]}, {transformed_end[1]});")
            elif isinstance(segment, QuadraticBezier):
                # Подобные трансформации и отрисовка
                pass

            elif isinstance(segment, Arc):

                start_point = self.apply_transform(segment.start)
                end_point = self.apply_transform(segment.end)
                center = self.apply_transform(segment.center)

                radius = np.linalg.norm(start_point[:2] - center[:2])
                start_angle = np.arctan2(start_point[1] - center[1], start_point[0] - center[0])
                end_angle = np.arctan2(end_point[1] - center[1], end_point[0] - center[0])

                if not segment.relative:
                    end_angle, start_angle = start_angle, end_angle
                if end_angle < start_angle:
                    end_angle += 2 * np.pi

                logging.info(
                    f"\tvoid gapi_arc(&gapi_hdl_{self.count}, {center[0]}, {center[1]}, {radius}, {start_angle}, {end_angle});"
                )


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
        if self.style_dict["stroke-dasharray"] == "none" and self.style_dict["fill"] == "none":
            logging.info(
                f"\tvoid gapi_ellipse(&gapi_hdl_{self.count}, {self.cx}, {self.cy}, {self.rx}, {self.ry});"
            )
        elif self.style_dict["fill"] != "none":
            logging.info(
                f"\tvoid gapi_circle_fill(&gapi_hdl_{self.count}, {self.cx}, {self.cy}, {self.rx});"
            )
        elif self.style_dict["stroke-dasharray"] != "none":
            logging.info(
                f"\tvoid gapi_circle_dash_arc(&gapi_hdl_{self.count}, {self.cx}, {self.cy}, {self.rx}, {self.style_dict['stroke-dasharray'].replace(',', '.')});"
            )


