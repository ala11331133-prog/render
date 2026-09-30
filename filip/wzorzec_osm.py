#!/usr/bin/env python3
# Czyta .osm normalnym parserem XML i zapisuje drogi, zeby test_odczyt
# mial z czym porownac TOsmOdczyt. Drogi dzieli tak samo jak DodajDroge.
# Uzycie: python3 wzorzec_osm.py map.osm map_wzorzec.txt

import sys
import xml.etree.ElementTree as ET


def main():
    if len(sys.argv) != 3:
        print("uzycie: wzorzec_osm.py plik.osm wynik.txt")
        return 1

    korzen = ET.parse(sys.argv[1]).getroot()

    wezly = {}
    for n in korzen.iter("node"):
        if "lat" in n.attrib and "lon" in n.attrib:
            wezly[n.attrib["id"]] = (n.attrib["lon"], n.attrib["lat"])

    with open(sys.argv[2], "wb") as out:
        for w in korzen.iter("way"):
            tagi = [(t.attrib["k"], t.attrib["v"]) for t in w.findall("tag")]

            kawalki, akt = [], []
            for nd in w.findall("nd"):
                p = wezly.get(nd.attrib["ref"])
                if p is not None:
                    akt.append(p)
                    continue
                if len(akt) >= 2:
                    kawalki.append(akt)
                akt = []
            if len(akt) >= 2:
                kawalki.append(akt)

            for punkty in kawalki:
                out.write(f"D {len(punkty)} {len(tagi)} {w.attrib['id']}\n".encode())
                for lon, lat in punkty:
                    out.write(f"P {lon} {lat}\n".encode())
                for k, v in tagi:
                    kb, vb = k.encode(), v.encode()
                    out.write(f"T {len(kb)} {len(vb)}\n".encode() + kb + vb + b"\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
