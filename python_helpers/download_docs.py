# This was generated by ChatGPT

import requests
from bs4 import BeautifulSoup
import random
import re
import os

num_docs_to_get = 100
min_words = 1000
min_words_before_strip = 1500
max_words = 2500

document_root_folder = 'DOCUMENTS'
output_folder = f"{document_root_folder}/wikipedia_documents"


def save_string_as_file(content, directory, file_name):
    file_path = os.path.join(directory, file_name + '.txt')
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)

def download_random_document():
    start_str = 'the free encyclopedia'
    start_str_len = len(start_str)

    end_str = 'page was last edited'
    end_str_len = len(end_str)

    url = "https://en.wikipedia.org/wiki/Special:Random"
    response = requests.get(url)
    if response.status_code == 200:
        soup = BeautifulSoup(response.content, 'html.parser')
        document = soup.get_text(separator=' ')
        document = re.sub('\s+', ' ', document)
        word_count = len(document.split())
        if word_count >= min_words_before_strip and word_count <= max_words:
            start_index = document.index('the free encyclopedia')
            end_index = document.index('page was last edited')
            document = document[start_index + start_str_len:end_index - end_str_len]
            word_count = len(document.split())
            if word_count > min_words:
                return document

document_number = 0
while document_number < num_docs_to_get:
    text = download_random_document()
    if text is not None:
        print(document_number)
        filename = "D-" + str(document_number)
        document_number += 1
        save_string_as_file(text, output_folder, filename)
