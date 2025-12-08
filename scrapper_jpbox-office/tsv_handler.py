import os

class TSV_handler:
    def __init__(self):
        self.filename = "french_films_box_office_in_france.tsv"
        self.link_file = "next_link.txt"
        self.default_url = "https://jpbox-office.com/v9_charts_total.php?view=2&filtre=classg&limite=0&infla=0&variable=0&tri=champ0&order=DESC&limit5=0"
        
        # Create TSV file if it doesn't exist
        if not os.path.exists(self.filename):
            self._create_tsv_file()
        
        # Create next_link.txt file if it doesn't exist
        if not os.path.exists(self.link_file):
            self._create_link_file()
    
    def _create_tsv_file(self):
        """Create TSV file with header"""
        with open(self.filename, 'w', encoding='utf-8') as f:
            f.write("title\tgenre\tdirector\trelease_year\tbox_office_france\n")
    
    def _create_link_file(self):
        """Create next_link.txt file with default URL"""
        with open(self.link_file, 'w', encoding='utf-8') as f:
            f.write(self.default_url)
    
    def add_film(self, film):
        """Add a film to the TSV file"""
        with open(self.filename, 'a', encoding='utf-8') as f:
            f.write("\t".join(str(item) for item in film) + "\n")
    
    def change_link(self, new_url):
        """Change the link in next_link.txt"""
        with open(self.link_file, 'w', encoding='utf-8') as f:
            f.write(new_url)
    
    def read_link(self):
        """Read the current link from next_link.txt"""
        with open(self.link_file, 'r', encoding='utf-8') as f:
            return f.read().strip()
