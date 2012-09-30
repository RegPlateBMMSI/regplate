# -*- coding: utf-8 -*-

import re
import sys
from operator import itemgetter

# offsety dla posczególnych linii w pliku z logami
plateNumberLineOffset = 0
expectedLettersLineOffset = 1
foundLettersLineOffset = 2
lettersDataLineOffset = 3


# szablon generowanego pliku
report_template = """
	<html>
		<head>
			<title>Projekt BMMSI</title>
			<meta charset="utf-8">
			<style type="text/css">
			body {
				font-family: "Calibri", "Trebuchet MS";
				padding-left: 100px;
				font-size: 11pt;
			}
			hr {
				color: sienna;
			}
			p {
				margin-left:20px;
			}
			img {
				max-width: 400px;
			}
			table {
				border-collapse: collapse;
				border: 1px solid black;
				font-family: "Arial";
				font-size: 10pt;
				cursor: default;
				text-indent: 0;
			}
			th {
				font-weight: normal;
			}
			tr:last-child {
				font-weight: bold;
			}
			td, th {
				text-align: center;
				width: 43px;
				border-bottom: 1px solid black;
				border-left: 2px solid black;
				border-right: 2px solid black;
				border-top: 1px solid black;
				height: 26px;
				vertical-align: middle;
			}
			
			td:nth-child(even) {
				background-color: #F7FCFF;
			}
			
			.character-frequency th {
				background-color: #F0F3FA;
			}

			.character-frequency th:nth-child(even) {
				background-color: #DDEFFA;
			}

			td:first-child, th:first-child {
				text-align: left;
				padding-right: 5px;
			}
			i {
				color: #06F;
			}
			
			</style>
		</head>
		<body>
			<h1>
				Raport z testowania sieci neuronowej 
				(%(num_layers)s / %(hidden_layers)s / %(desired_error)s
				/ %(train_count)d / %(test_count)d)
			</h1>
			<div class="report-header">
				<h3>Parametry sieci</h3>
				<ul>
					<li>
						Liczba warstw: <strong><i>%(num_layers)s</i></strong>
					</li>
					<li>
						Ukrytych neuronów: <strong><i>%(hidden_layers)s</i></strong>
					</li>
					<li>
						Oczekiwany błąd podczas uczenia: <strong><i>%(desired_error)s</i></strong>
					</li>
				</ul>
				<h3>Dane testowe</h3>
				<ul>
					<li>
						Tablic uczących: <strong><i>%(train_count)d</i></strong>
					</li>
					<li>
						Tablic testowych: <strong><i>%(test_count)d</i></strong>
					</li>
				</ul>
				<h3>Wyniki</h3>
				<ul>
					<li>
						Poprawnie wykryto znaki na tablicach: <strong>%(recognized_plates_count)d</strong>
						z %(test_count)d
						(<em>%(recognized_plates_count_percent).1f%%</em>)
					</li>
					<li>
						Poprawnie rozpoznano znaków: <strong>%(recognized_characters_count)d</strong>
						z %(characters_count)d
						(<em>%(recognized_characters_count_percent).1f%%</em>)
					</li>
				</ul>
			</div>
			<h2>Rozpoznawalność znaków</h2>
			<div class="report-body">
				%(character_frequency)s
			</div>
			<h2>Wyniki rozpoznawania tablic</h2>
			<div class="report-body">
				%(report_body)s
			</div>
		</body>
	</html>
"""

# szablon dla szczegółów pojedynczej tablicy
single_report = """
	<h4>Tablica %(plate_number)s</h4>
	<p><img src="%(photo_url)s" title="Obrazek z tablicą"/></p>
	<div class="report-single">
	%(body)s
	</div>
"""	

# szablon dla tablicy, z której nie udało się odczytać znaków
not_matched_error = """
	<div class="not-matched">
		Nie wykryto wszystkich znaków na tablicy. Rozpoznawanie znaków przy użyciu sieci zostalo anulowane.
	</div>
"""

LETTER_NOT_RECOGNIZED = "?"

# dopuszczalne znaki. Musi zgadzać się z tą z pliku commons/ConversionTools.cpp
possible_characters = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
	'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9']


def parseAllReports(reports):
	"""
	Zwraca szablon raportu uzupełniony o wymagane pola
	"""
	 # rozdziela plik z raportami według wzorca końca raportu i dla każdego wywołuje funkcję go parsującą
	report_results = map(parseSingleReport, reports.split("Processing finished\n")[:-1])
	test_count = len(report_results)
	train_count = readTrainDataCount()
	# oblicza liczbę tablic, które udało się rozpoznać
	recognized_plates_count = len(filter(itemgetter("plate_recognized"), report_results))
	# tworzy listę wszystkich znaków które pojawiły sie na tablicach
	all_characters = reduce(adder, map(itemgetter("all_characters"), report_results))
	# tworzy listę wszystkich znaków które dało się rozpoznać
	recognized_characters = reduce(adder, map(itemgetter("recognized_characters"), report_results))
	report_body = reduce(adder, map(itemgetter("html"), report_results)) # tworzy treść raportu z pojedynczych raportów dla tablic
	
	# pobiera oczekiwany błąd, liczbę warstw i liczbę neuronów na warstwach ukrytych podane jako parametry programu
	desired_error = sys.argv[1]
	num_layers = sys.argv[2]
	hidden_layers = ", ".join(sys.argv[3:])

	# generuje raport na podstawie przekazanych zmiennych
	return report_template % {
		"desired_error": desired_error,
		"num_layers": num_layers,
		"hidden_layers": hidden_layers,
		"test_count": test_count,
		"train_count": train_count,
		"recognized_plates_count": recognized_plates_count,
		"recognized_plates_count_percent": getPercentOrZero(recognized_plates_count, test_count),
		"recognized_characters_count": len(recognized_characters),
		"characters_count": len(all_characters),
		"recognized_characters_count_percent": getPercentOrZero(len(recognized_characters), len(all_characters)),
		"character_frequency": generateCharacterFrequency(recognized_characters, all_characters),
		"report_body": report_body
	}
	
def readTrainDataCount():
	""" Odczytuje ile wierszy znajduje się w pliku z danymi treningowymi """
	with open("traindata.txt") as train_data:
		return len(train_data.readlines())
	
adder = lambda x,y: x + y

def getPercentOrZero(factor, all):
	""" Zabezpiecza przed podzieleniem przez 0 """
	if all > 0:
		return factor  * 100 / float(all)
	else:
		return 0
	
def generateCharacterFrequency(recognized_characters, all_characters):
	"""
	Tworzy tabelkę ze statystyką ilukrotnie znak pojawił się w danych treningowych, testowych
	i z jaką dokładnością był rozpoznawany w danych testowych.
	"""
	recognized_characters_histogram = makeCharactersHistogram(recognized_characters)
	all_characters_histogram = makeCharactersHistogram(all_characters)
	train_charactes_histogram = makeCharactersHistogram(readTrainData())
	
	def generate_table(characters_range):
		""" Domknięcie dla funkcji generateCharacterFrequencyForCharactersRange"""
		return generateCharacterFrequencyForCharactersRange(characters_range, all_characters_histogram, recognized_characters_histogram,
			train_charactes_histogram)
		
	return generate_table(possible_characters[26:]) + "<br/>" + generate_table(possible_characters[0:26])

def readTrainData():
	"""
	Odczytuje z danych w formacie FANN, jakie litery pojawiały się w danych testowych
	"""
	train_characters = []
	with open("regplate.train") as train_data:
		result_lines = train_data.readlines()[1:][1::2]
		for line in result_lines:
			train_characters.append(possible_characters[line.index("1") / 2])
	return train_characters

def makeCharactersHistogram(characters):
	"""
	Tworzy prosty histogram z częstością występowania dla podanej listy znaków
	"""
	histogram = dict()
	for character in characters:
		histogram[character] = histogram.setdefault(character, 0) + 1
	return histogram
		
def generateCharacterFrequencyForCharactersRange(characters_range, all_characters_histogram,
		recognized_characters_histogram, train_charactes_histogram):
	"""
	Tworzy tabelkę ze statystyką wystąpień znaków z listy characters_range (np. 0-9 lub A-Z)
	"""
	table = "<table class='character-frequency tabelka'>"

	table += "<thead>"
	table += "<th>Znak</th>"
	for letter in characters_range:
		table += "<th>%s</th>" % letter
	table += "</thead>"

	table += "<tbody>"
	table += "<tr>"

	table += "<td>W danych treningowych</td>"
	for letter in characters_range:
		table += "<td>%s</td>" % train_charactes_histogram.setdefault(letter, 0)
	table += "</tr><tr>"

	table += "<td>W danych testowych</td>"
	for letter in characters_range:
		table += "<td>%s</td>" % all_characters_histogram.setdefault(letter, 0)
	table += "</tr><tr>"

	table += "<td>% Trafień</td>"
	for letter in characters_range:
		if all_characters_histogram[letter] == 0:
			table += "<td style='background-color: #dddddd;'></td>"
		else:
			ratio = getPercentOrZero(recognized_characters_histogram.setdefault(letter, 0),
					all_characters_histogram[letter])
			table += "<td style='background-color: %s;'>%.0f</td>" % (getColorForPropability(ratio/100.), ratio)
	table += "</tr>"
	table += "</tbody>"
	table += "</table>"
	return table
	
def parseSingleReport(report):
	"""
	Dokonuje parsowania raportu dla pojedynczej tablicy testowej. Zwraca słownik z informacjami o:
	 - tym, czy znaki na tablicy zostały znalezione przez algorytm rozpoznawania obrazów
	 - listę poprawnie rozpoznanych przez FANN znaków
	 - wszystkie znaki, które pojawiły się na tablicy
	 - tekst z raportem w HTMLu
	"""
	print "*",
	lines = report.splitlines()

	plate_recognized = foundAllLetters(lines)
	if plate_recognized:
		lettersData = map(getMatchDataForLetter, lines[3:])
		recognized_characters = map(itemgetter("expectedLetter"), filter(characterIsRecognized, lettersData))
		all_characters = map(itemgetter("expectedLetter"), lettersData)
		body = generateTable(lettersData)
	else:
		recognized_characters = []
		all_characters = []
		body = not_matched_error

	return {
		"plate_recognized": plate_recognized,
		"recognized_characters": recognized_characters,
		"all_characters": all_characters,
		"html": single_report % {
			"plate_number": getPlateNumber(lines),
			"body": body,
			"photo_url": "../photos/%s.jpg" % getPlateNumber(lines)
		}
	}

characterIsRecognized = lambda x: x["expectedLetter"] == x["resultLetter"]
	
def getPlateNumber(txt):
	""" Odczytuje z logu numer tablicy """
	return re.search("Processing file: \t(.*?)$", txt[plateNumberLineOffset]).group(1)

def foundAllLetters(txt):
	""" Odczytuje z logu oczekiwaną liczbę znaków i liczbę znaków znalezionych i na tej podstawie określa, czy znaleziono wszystkie znaki"""
	expectedLetters = re.search("Expected letters: \t(.*?)$", txt[expectedLettersLineOffset]).group(1)
	foundLetters = re.search("Found letters: \t(.*?)$", txt[foundLettersLineOffset]).group(1)
	return expectedLetters == foundLetters
	
def getMatchDataForLetter(text):
	""" Odczytuje z logu informację o szukanym znaku, znaku rozpoznanym przez sieć i prawdopodobieństwi, że to te same znaki """
	match = re.search("Best match for letter (.*?) is (.*?) with propability (.*?)$", text)
	if match is not None:
		expectedLetter, resultLetter, propability = match.groups()
		return {
			'expectedLetter': expectedLetter,
			'resultLetter': resultLetter,
			'propability': float(propability)
		}
	else:
		expectedLetter = re.search("No match for letter (.*?)$", text).group(1)
		return {
			'expectedLetter': expectedLetter,
			'resultLetter': LETTER_NOT_RECOGNIZED,
			'propability': 0
		}

def generateTable(lettersData):
	""" Tworzy tabelkę w HTML zawierającą porównanie oczekiwanej litery z otrzymaną """
	table = "<table>"

	table += "<thead>"
	table += "<th>Oczekiwano</th>"
	for letterData in lettersData:
		table += "<th style='%s'>%s</th>" % (cssStyleForLetterData(letterData), letterData["expectedLetter"])
	table += "</thead>"

	table += "<tbody>"
	table += "<tr>"
	table += "<td>Otrzymano</td>"
	for letterData in lettersData:
		table += "<td style='%s'>%s</td>" % (cssStyleForLetterData(letterData), letterData["resultLetter"])
	table += "</tr><tr>"
	table += "<td>Prawdopodobieństwo</td>"
	for letterData in lettersData:
		table += "<td style='%s'>%.2f</td>" % (cssStyleForLetterData(letterData), letterData["propability"])
	table += "</tr>"
	table += "</tbody>"
	table += "</table>"
	return table
	
def cssStyleForLetterData(letterData):
	"""
	Na podstawie tego, czy znak został rozpoznany poprawnie i z jakim prawdopodobieństwem,
	zwraca odpowiedną styl CSS, służący zmiany koloru tła znaku w tabeli
	"""
	color = "background-color: %s; "
	propability = letterData["propability"]
	if letterData["resultLetter"] == LETTER_NOT_RECOGNIZED:
		return color % "grey"
	elif letterData["expectedLetter"] != letterData["resultLetter"]:
		if propability > 0.3:
			return ";".join([color % "#FF008F", "font-style: italic;"]);
		else:
			return ";".join([color % "#FC7BAE", "font-style: italic;"]);
	else:
		return color % getColorForPropability(propability)

def getColorForPropability(propability):
	""" W zależności od prawdopodobieństwa zwraca kolor ze skali od zielonego do czerwonego"""
	if propability > 0.999:
		return "#4BFD5E"
	if propability > 0.9:
		return "#BBFD67"
	if propability > 0.8:
		return "#D6FD5B"
	if propability > 0.7:
		return "#DAFF8C"
	if propability > 0.6:
		return "#E8FF8C"
	if propability > 0.5:
		return "#FFFA8C"
	if propability > 0.4:
		return "#FFDF8C"
	if propability > 0.3:
		return "#FFA96F"
	if propability > 0.2:
		return "#FF7635"
	return "#FF2F2F"


if __name__ == "__main__":
	""" Generuje z pliku report.txt raport do pliku report.html """
	with open("report.txt") as input:
		with open("report.html", "w+") as output:
			output.write(parseAllReports(input.read()))
			print "OK"
	
		