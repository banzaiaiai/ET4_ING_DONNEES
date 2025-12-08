from scrapper import scrape_website
from tsv_handler import TSV_handler

if __name__ == "__main__":
    begining = 0
    tsv_handler = TSV_handler()

    while 1:
        (new_url, films) = scrape_website(tsv_handler.read_link())
        tsv_handler.change_link(new_url)
        for film in films:
            tsv_handler.add_film(film)

        if new_url == "No next page" :
            break
 
