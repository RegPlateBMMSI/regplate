# -*- coding: utf-8 -*-

import re
from operator import itemgetter

plateNumberLineOffset = 0
expectedLettersLineOffset = 1
foundLettersLineOffset = 2
lettersDataLineOffset = 3



reports_header = """
	<html>
		<head>
			<title>Projekt BMMSI</title>
			<meta charset="utf-8">
			<style type="text/css">
			body {
				font-family: "Trebuchet MS";
				padding-left: 100px;
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
				font-size: 14px;
				cursor: default;
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
			}
			td:first-child, th:first-child {
				text-align: left;
				padding-right: 5px;
			}
			i {
				color: red;
			}
			
			</style>
		</head>
		<body>
			<h1>Raport z testowania sieci neuronowej</h1>
			<div class="report-header">
				<h3>Parametry sieci</h3>
				<p>
					Liczba warstw: <strong><i>uzupełnić</i></strong>
				</p>
				<p>
					Ukrytych neuronów: <strong><i>uzupełnić</i></strong>
				</p>
				<p>
					Oczekiwany błąd podczas uczenia: <strong><i>uzupełnić</i></strong>
				</p>
				<h3>Dane testowe</h3>
				<p>
					Tablic uczących: <strong><i>uzupełnić</i></strong>
				</p>
				<p>
					Tablic testowych: <strong>%(test_count)d</strong>
				</p>
				<h3>Wyniki</h3>
				<p>
					Poprawnie wykryto znaki na tablicach: <strong>%(recognized_plates_count)d</strong>
					z %(test_count)d
					(<em>%(recognized_plates_count_percent).1f%%<em>)
				</p>
				<p>
					Poprawnie rozpoznano znaków: <strong>%(recognized_characters_count)d</strong>
					z %(characters_count)d
					(<em>%(recognized_characters_count_percent).1f%%<em>)
				</p>
			</div>
			<h1>Rozpoznawalność liter</h1>
			<div class="report-body">
				%(character_frequency)s
			</div>
			<h1>Wyniki rozpoznawania tablic</h1>
			<div class="report-body">
				%(report_body)s
			</div>
		</body>
	</html>
"""


single_report = """
	<h4>Tablica %(plate_number)s</h4>
	<p><img src="%(photo_url)s" title="Obrazek z tablicą"/></p>
	<div class="report-single">
	%(body)s
	</div>
"""	


not_matched_error = """
	<div class="not-matched">
		Nie wykryto wszystkich znaków na tablicy. Rozpoznawanie znaków przy użyciu sieci zostalo anulowane.
	</div>
"""

def parseAllReports(reports):
	report_results = map(parseSingleReport, reports.split("Processing finished\n")[:-1])
	test_count = len(report_results)
	recognized_plates_count = len(filter(itemgetter("plate_recognized"), report_results))

	recognized_characters = reduce(adder, map(itemgetter("recognized_characters"), report_results))
	all_characters = reduce(adder, map(itemgetter("all_characters"), report_results))
	report_body = reduce(adder, map(itemgetter("html"), report_results))

	return reports_header % {
		"test_count": test_count,
		"recognized_plates_count": recognized_plates_count,
		"recognized_plates_count_percent": getPercentOrZero(recognized_plates_count, test_count),
		"recognized_characters_count": len(recognized_characters),
		"characters_count": len(all_characters),
		"recognized_characters_count_percent": getPercentOrZero(len(recognized_characters), len(all_characters)),
		"character_frequency": generateCharacterFrequency(recognized_characters, all_characters),
		"report_body": report_body
	}
	
adder = lambda x,y: x + y

def getPercentOrZero(factor, all):
	if all > 0:
		return factor  * 100 / float(all)
	else:
		return 0
	
def parseSingleReport(report):
	print "*",
	lines = report.splitlines()

	plate_recognized = foundAllLetters(lines)
	if plate_recognized:
		lettersData = map(getMatchDataForLetter, lines[3:-1])
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
	return re.search("Processing file: \t(.*?)$", txt[plateNumberLineOffset]).group(1)

def foundAllLetters(txt):
	expectedLetters = re.search("Expected letters: \t(.*?)$", txt[expectedLettersLineOffset]).group(1)
	foundLetters = re.search("Found letters: \t(.*?)$", txt[foundLettersLineOffset]).group(1)
	return expectedLetters == foundLetters
	
def getMatchDataForLetter(text):
	expectedLetter, resultLetter, propability = re.search("Best match for letter (.*?) is (.*?) with propability (.*?)$", text).groups()
	return {
		'expectedLetter': expectedLetter,
		'resultLetter': resultLetter,
		'propability': float(propability)
	}

def generateTable(lettersData):
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
	color = "background-color: %s; "
	propability = letterData["propability"]
	if letterData["expectedLetter"] != letterData["resultLetter"]:
		if propability > 0.3:
			return ";".join([color % "#FF008F", "font-style: italic;"]);
		else:
			return color % "red";
	else:
		return color % getColorForPropability(propability)

		
def generateCharacterFrequency(recognized_characters, all_characters):
	possible_letters = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
	'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
	
	recognized_characters_histogram = dict()
	all_characters_histogram = dict()

	for character in recognized_characters:
		recognized_characters_histogram[character] = recognized_characters_histogram.setdefault(character, 0) + 1
	for character in all_characters:
		all_characters_histogram[character] = all_characters_histogram.setdefault(character, 0) + 1
	
	table = "<table>"

	table += "<thead>"
	table += "<th>Litera</th>"
	for letter in possible_letters:
		table += "<th>%s</th>" % letter
	table += "</thead>"

	table += "<tbody>"
	table += "<tr>"
	table += "<td>Wystąpień</td>"
	for letter in possible_letters:
		table += "<td>%s</td>" % all_characters_histogram.setdefault(letter, 0)
	table += "</tr><tr>"
	table += "<td>% Trafień</td>"
	for letter in possible_letters:
		if all_characters_histogram.setdefault(letter, 0) == 0:
			table += "<td style='background-color: #dddddd;'></td>"
		else:
			ratio = getPercentOrZero(recognized_characters_histogram.setdefault(letter, 0),
					all_characters_histogram.setdefault(letter, 0))
			table += "<td style='background-color: %s;'>%.1f</td>" % (getColorForPropability(ratio/100.), ratio)
	table += "</tr>"
	table += "</tbody>"
	table += "</table>"
	return table
	


def getColorForPropability(propability):
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
	with open("report.txt") as input:
		with open("report.html", "w+") as output:
			output.write(parseAllReports(input.read()))
			print "OK"
	
		