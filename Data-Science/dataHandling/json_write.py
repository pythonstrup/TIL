import json

dict_data = {"Name": "Kaka", "Age": 24, "Class": "First"}

with open("json/data1.json", "w") as f:
    json.dump(dict_data, f)