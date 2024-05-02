from source.logger import setup_logging
from source.svg_parser import ParseSVG


def main():
    setup_logging("cache.log")
    parse = ParseSVG("files/Маршрут/ППМ.svg")
    parse.start()


if __name__ == '__main__':
    main()