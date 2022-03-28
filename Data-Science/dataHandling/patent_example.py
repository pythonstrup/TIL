import urllib.request
from bs4 import BeautifulSoup

with open("xml/US08621662-20140107.XML", "r", encoding="utf8") as patent_xml:
    xml = patent_xml.read()

soup = BeautifulSoup(xml, "lxml")  # lxml parser 호출

invention_title_tag = soup.find("invention-title")
print(invention_title_tag)