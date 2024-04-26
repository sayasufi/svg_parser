import xml.etree.ElementTree as ET

# Путь к SVG файлу
svg_file_path = "files/Объекты АНИ/Аэродромы совместного базирования.svg"

# Загрузка SVG файла
tree = ET.parse(svg_file_path)
root = tree.getroot()

# Функция для извлечения данных из тегов path, ellipse, circle
def extract_data_from_element(element):
    tag = element.tag
    if tag == "{http://www.w3.org/2000/svg}path":
        # Обработка данных из тега path
        path_data = element.get("d")
        print("Path data:", path_data)
    elif tag == "{http://www.w3.org/2000/svg}ellipse":
        # Обработка данных из тега ellipse
        cx = element.get("cx")
        cy = element.get("cy")
        rx = element.get("rx")
        ry = element.get("ry")
        print("Ellipse data: cx={}, cy={}, rx={}, ry={}".format(cx, cy, rx, ry))
    elif tag == "{http://www.w3.org/2000/svg}circle":
        # Обработка данных из тега circle
        cx = element.get("cx")
        cy = element.get("cy")
        r = element.get("r")
        print("Circle data: cx={}, cy={}, r={}".format(cx, cy, r))

# Проход по всем элементам SVG файла
for child in root:
    extract_data_from_element(child)
