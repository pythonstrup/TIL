import csv

data = []
header = []
rownum = 0

with open("csv/korea_traffic_data.csv", "r", encoding="cp949") as file:
    csv_data = csv.reader(file)
    for row in csv_data:
        if rownum == 0:
            header = row

        try:
            location = row[4]
        except IndexError:
            continue

        if location.find(u"안산시") != -1:  # u -> 유니코드
            data.append(row)
        rownum += 1

with open("csv/Ansan_traffic_data.csv", "w", encoding="utf-8") as a_file:
    writer = csv.writer(a_file, delimiter="\t", quotechar="'", quoting=csv.QUOTE_ALL)
    writer.writerow(header)
    for row in data:
        writer.writerow(row)