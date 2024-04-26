import xml.etree.ElementTree as ET


def parse_svg_metadata(svg_file):
    tree = ET.parse(svg_file)
    root = tree.getroot()

    metadata = {}

    for elem in root.iter():
        if 'id' in elem.attrib:
            metadata[elem.attrib['id']] = {
                'tag': elem.tag,
                'attributes': elem.attrib
            }

    return metadata


svg_file = 'files/Маршрут/Аэродром.svg'  # Укажите путь к вашему SVG файлу
metadata = parse_svg_metadata(svg_file)

for element_id, data in metadata.items():
    print(f"Element ID: {element_id}")
    print(f"Tag: {data['tag']}")
    print("Attributes:")
    for key, value in data['attributes'].items():
        print(f"  {key}: {value}")
    print()