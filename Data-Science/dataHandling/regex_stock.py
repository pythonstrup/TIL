import re
import urllib.request

url = "https://finance.naver.com/item/main.naver?code=005930"
html = urllib.request.urlopen(url)
html_contents = str(html.read().decode("ms949"))

# result = re.findall("(\<dl class=\"blind\"\>)([\s\S]+?)(\<\/dl\>)", html_contents)
result = re.findall('(<dl class="blind">)([\s\S]+?)(</dl>)', html_contents)
stock = result[0]
index = result[1]

stock_list = re.findall("(\<dd\>)([\s\S]+?)(\<\/dd\>)", str(stock))
for st in stock_list:
    print(st)

index_list = re.findall("(\<dd\>)([\s\S]+?)(\<\/dd\>)", str(index))
for idx in index_list:
    print(idx[1])