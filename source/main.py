import os

from source.logger import setup_logging
from source.svg_parser import ParseSVG


def main():
    setup_logging("cache.log")
    # Список директорий для поиска файлов SVG
    directories = ["files/Маршрут", "files/Объекты АНИ"]
    # Проход по всем директориям
    for directory in directories:
        # Проход по всем файлам в директории
        for filename in os.listdir(directory):
            # Полный путь к файлу
            file_path = os.path.join(directory, filename)
            # Проверка, что файл является файлом SVG
            if file_path.endswith('.svg'):
                # Вызов функции с полным путём к файлу SVG
                parse = ParseSVG(file_path)
                parse.start()


if __name__ == '__main__':
    main()
