import logging
import os
import xml.etree.ElementTree as ET

from source.types import Path, Ellipse

dict_types = {"path": Path, "ellipse": Ellipse}


class ParseSVG:
    def __init__(self, svg_file_path):
        # Путь к SVG файлу
        self.svg_file_path = svg_file_path
        file_name = os.path.basename(self.svg_file_path)
        logging.info("\n" + file_name + "\n")

        # Загрузка SVG файла
        tree = ET.parse(svg_file_path)
        self.root = tree.getroot()

        self.count = 0

    def start(self):
        for element in self.find_all_tags(self.root):
            self.extract_data_from_element(element)

    def extract_data_from_element(self, element):
        tag = element.tag.split("}")[1]
        temp_class = dict_types[tag](element, self.count)
        temp_class.start()
        self.count += 1

    def find_all_tags(self, element):
        paths_and_ellipses = []
        if element.tag.split("}")[1] in dict_types.keys():
            paths_and_ellipses.append(element)
        for child in element:
            paths_and_ellipses.extend(self.find_all_tags(child))
        return paths_and_ellipses
