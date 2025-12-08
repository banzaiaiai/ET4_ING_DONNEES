import requests
from bs4 import BeautifulSoup

def scrape_website(url):
    print(f"Getting : {url}")
    response = requests.get(url)
    if response.status_code == 200:
        print("Extracting from HTML")
        return get_usefull_data(response.content)
    else:
        print(f"Failed to retrieve the webpage. Status code: {response.status_code}")

def fix_encoding(text):
    """Corrige les problèmes d'encodage courants"""
    if not text:
        return ''
    
    # Dictionnaire des corrections d'encodage
    corrections = {
        'Ã©': 'é',
        'Ã¨': 'è',
        'Ã ': 'à',
        'Ã®': 'î',
        'Ã´': 'ô',
        'Ã¢': 'â',
        'Ã§': 'ç',
        'Ã¹': 'ù',
        'Ãª': 'ê',
        'Ã«': 'ë',
        'Ã¯': 'ï',
        'Ã»': 'û',
        'Ã¼': 'ü',
        'Å"': 'œ',
        'â\x80\x99': "'",  # Apostrophe spéciale
        '\'': "\'",
    }
    
    for bad, good in corrections. items():
        text = text.replace(bad, good)
    
    return text

def parse_box_office(text):
    """Convertit le box office en int"""
    if not text:
        return 0
    # Supprimer les espaces et convertir en int
    cleaned = text.replace(' ', '').replace('\u00a0', '').strip()
    try:
        return int(cleaned)
    except ValueError:
        return 0

def get_usefull_data(html_content):
    soup = BeautifulSoup(html_content, 'html.parser')
    # On récupère les lignes <tr> du tableau
    rows = soup.find_all('tr')
    
    films = []
    
    for row in rows:
        # Vérifier que la ligne contient bien les données d'un film
        titre_td = row.find('td', attrs={"class": "col_poster_titre"})
        if not titre_td:
            continue
        
        # Titre français (dans le h3 > a)
        titre_tag = titre_td. find('h3')
        titre = titre_tag. find('a'). get_text(strip=True) if titre_tag and titre_tag.find('a') else ''
        titre = fix_encoding(titre)
        
        # Genre
        genre = ''
        genre_tag = titre_td.find('a', href=lambda x: x and 'filtre=genre' in x)
        if genre_tag:
            genre = fix_encoding(genre_tag.get_text(strip=True))
        
        # Réalisateur
        realisateur = ''
        realisateur_tag = titre_td.find('a', href=lambda x: x and 'fichacteur' in x)
        if realisateur_tag:
            realisateur = fix_encoding(realisateur_tag.get_text(strip=True))
        
        # Récupérer toutes les colonnes col_poster_contenu
        contenu_tds = row.find_all('td', attrs={"class": "col_poster_contenu"})
        
        # Année de sortie (première col_poster_contenu)
        annee_sortie = ''
        if len(contenu_tds) >= 1:
            annee_tag = contenu_tds[0].find('a')
            if annee_tag:
                annee_sortie = annee_tag.get_text(strip=True)
        
        # Box office France (col_poster_contenu_majeur)
        box_office_france = 0
        box_office_td = row.find('td', attrs={"class": "col_poster_contenu_majeur"})
        if box_office_td:
            box_office_france = parse_box_office(box_office_td.get_text(strip=True))
        
        # Ignorer les lignes vides (sans titre)
        if not titre:
            continue
        
        film = [
            titre,
            genre,
            realisateur,
            annee_sortie,
            box_office_france
        ]
        
        films.append(film)

    next_url_page = soup.find('div', attrs={"class": "pagination"}).find_all('a')
    next_url_page = list(filter(lambda a : a.getText() == ">", next_url_page))
    next_url_page = "https://jpbox-office.com/" + next_url_page[0].attrs['href'] if next_url_page else "No next page"
    
    return next_url_page, films
