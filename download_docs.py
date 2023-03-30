import requests
from bs4 import BeautifulSoup
import random
import re

def download_random_document():
    url = "https://en.wikipedia.org/wiki/Special:Random"
    response = requests.get(url)
    print(response.status_code)
    if response.status_code == 200:
        soup = BeautifulSoup(response.content, 'html.parser')
        document = soup.get_text(separator=' ')
        document = re.sub('\s+', ' ', document)
        word_count = len(document.split())
        if word_count >= 1000:
            return document

documents = []
for i in range(100):
    document = download_random_document()
    print(document)
    documents.append(document)

# do something with the documents here