#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>//pt citirea din fisiere 
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h> 

#define ALPHABET_SIZE 26
#define TOP_N 3

void read_distribution(const char* filename, double distribution[ALPHABET_SIZE]) {
	FILE* file = fopen(filename, "r");
	char letter;
	float value;

	while (fscanf(file, "%c %f\n", &letter, &value) == 2) {
		if (letter >= 'a' && letter <= 'z') {
			distribution[letter - 'a'] = value;
		}
		else if (letter >= 'A' && letter <= 'Z') {
			distribution[letter - 'A'] = value;
		}
	}
	/*for (int i = 0;i < 26;i++)
		printf("%f\n", distribution[i]);*/
	fclose(file);
}

void compute_histogram(const char* text, double histogram[ALPHABET_SIZE]) {
	int frequency[ALPHABET_SIZE] = { 0 };
	int total = 0;
	for (int i = 0;text[i] != '\0';i++) {
		char c = tolower(text[i]);
		if (c >= 'a' and c <= 'z') {
			frequency[c - 'a']++;
			total += 1;
		}
	}
	for (int i = 0;i < ALPHABET_SIZE;i++)
		if (total > 0) {
			histogram[i] = (double)frequency[i] / total;
		}
		else {
			histogram[i] = 0;
		}

}

double chi_squared_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
	double distance = 0.0;
	for (int i = 0;i < ALPHABET_SIZE;i++) {
		distance += (double)((hist1[i] - hist2[i]) * (hist1[i] - hist2[i]) / hist2[i]);
	}
	return distance;

}
double cosine_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
	double dot = 0, prod_hist1 = 0, prod_hist2 = 0;
	for (int i = 0;i < ALPHABET_SIZE;i++) {
		dot += hist1[i] * hist2[i];
		prod_hist1 += hist1[i] * hist1[i];
		prod_hist2 += hist2[i] * hist2[i];
	}
	return 1 - dot / (sqrt(prod_hist1) * sqrt(prod_hist2));

}
double euclidian_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
	double squared_difference = 0;
	for (int i = 0;i < ALPHABET_SIZE;i++) {
		squared_difference += (hist1[i] - hist2[i]) * (hist1[i] - hist2[i]);
	}
	return sqrt(squared_difference);
}

void encrypt_text(char* text, int shift) {
	for (int i = 0; text[i] != '\0'; i++) {
		char c = text[i];
		if (c >= 'a' && c <= 'z') {
			text[i] = ((c - 'a' + shift) % 26) + 'a';
		}
		else if (c >= 'A' && c <= 'Z') {
			text[i] = ((c - 'A' + shift) % 26) + 'A';
		}
	}
}

void decrypt_text(char* text, int shift) {
	for (int i = 0; text[i] != '\0'; i++) {
		char c = text[i];
		if (c >= 'a' && c <= 'z') {
			if (c - shift >= 'a') c = c - shift;
			else {
				shift -= (c - 'a');
				c = 'z' - shift;
			}
		}
		else if (c >= 'A' && c <= 'Z') {
			if (c - shift >= 'A') c = c - shift;
			else {
				shift -= (c - 'A');
				c = 'Z' - shift;
			}
		}
		text[i] = c;
	}
}

void break_caesar_cipher(const char* text, int top_shifts[TOP_N], double top_distances[TOP_N], double(*distance_function)(const double[], const double[])) {
	double distribution[ALPHABET_SIZE];
	read_distribution("distribution.txt", distribution);

	for (int i = 0;i < TOP_N;i++) {
		top_distances[i] = INFINITY;
		top_shifts[i] = -1;
	}
	int cnt = 0;

	for (int shift = 0;shift < ALPHABET_SIZE;shift++) {
		char shifted_text[1001];
		strcpy(shifted_text, text);

		/*for (int i = 0;shifted_text[i] != '\0';i++) {
			if (shifted_text[i] >= 'a' and shifted_text[i] <= 'z') {
				shifted_text[i] = 'a' + (shifted_text[i]-'a' + shift) % 26;
			}
			else if (shifted_text[i] >= 'A' and shifted_text[i] <= 'Z') {
				shifted_text[i] = 'A' + (shifted_text[i] - 'A' + shift) % 26;
			}
		}*/
		encrypt_text(shifted_text, shift);
		double hist[ALPHABET_SIZE];
		compute_histogram(shifted_text, hist);
		
		double distance = distance_function(hist, distribution);
		for (int i = 0;i < TOP_N;i++) {
			if (distance < top_distances[i]) {
				cnt++;
				/*printf("%d\n", cnt);*/
				for (int j = TOP_N - 1;j > i;j--)
				{
					top_distances[j] = top_distances[j - 1];
					top_shifts[j] = top_shifts[j - 1];
				}
				top_distances[i] = distance;
				top_shifts[i] = shift;
				break;
			}
		}

	}

}

void menu() {
	char text[1001];
	int choice;
	int inserted_text = 0;
	printf("Menu:\n1.Enter text from keyboard.\n2.Read text from a file.\n3.Encrypt a text with a specific shift.\n4.Decrypt a text with a known shift.\n5.Compute and display the frequency distribution of a text.\n6.Breaking the encrypted text using frequency analysis.");
	while (true) {
		printf("\nYour choice:");
		scanf("%d", &choice);
		getchar();

		if (choice == 1) {
			printf("Enter text: ");
			fgets(text, 1001, stdin);
			text[strcspn(text, "\n")] = 0;
			inserted_text = 1;
			printf("The text inserted: %s\n", text);
		}
		else if (choice == 2) {//Daca fisierul va avea mai multe linii se for concatena
			char filename[101];
			FILE* file;
			printf("Enter the filename: ");
			scanf("%s", filename);

			file = fopen(filename, "r");

			if (file == NULL) {
				printf("Error opening the file %s\n:", filename);
				exit(0);
			}
			char line_text[1001];
			int cnt = 0;
			while (fgets(line_text, sizeof(line_text), file)) {
				int i = 0;
				while (line_text[i]) {
					text[cnt++] = line_text[i++];
				}
			}

			text[cnt] = '\0';
			inserted_text = 1;
			printf("The text in the file is:\n%s\n", text);
			fclose(file);
		}
		else if (choice == 3) {
			if (inserted_text) {
				int shift;
				printf("Enter shift: ");
				scanf("%d", &shift);
				shift %= 26;
				encrypt_text(text, shift);
				printf("Encrypted text: %s\n", text);
			}
			else printf("First,enter a text!");
		}
		else if (choice == 4) {
			if (inserted_text) {
				int shift;
				printf("Enter shift: ");
				scanf("%d", &shift);
				shift %= 26;
				decrypt_text(text, shift);
				printf("Decrypted text: %s\n", text);
			}
			else printf("First,enter a text!");
		}
		else if (choice == 5) {
			double histo[ALPHABET_SIZE];
			if (inserted_text) {
				compute_histogram(text, histo);
				for (int i = 0;i < ALPHABET_SIZE;i++)
					if (histo[i])
						printf("The letter %c has a occurrence of %.2f%%\n", 'a' + i, histo[i]);
			}
			else printf("First,enter a text!");
		}
		else if (choice == 6) {
			if (inserted_text) {
				printf("Select a distance metric(Chi-Squared Distance-->1,Cosine Distance-->2,Euclidean Distance-->3): ");
				int metric = 0;
				scanf("%d", &metric);
				int top_shifts[TOP_N];
				double top_distances[TOP_N];
				if (metric == 1) {
					break_caesar_cipher(text, top_shifts, top_distances, chi_squared_distance);
				}
				else if (metric == 2) {
					break_caesar_cipher(text, top_shifts, top_distances, cosine_distance);
				}
				else if (metric == 3) {
					break_caesar_cipher(text, top_shifts, top_distances, euclidian_distance);
				}
				/*printf("The most likely shift based on the lowest computed distance is %d:", top_shifts[0]);*/
				printf("The most likely shifts based on the lowest computed distance are:\n");
				for (int i = 0;i < 3;i++)
					printf("Shift %d with a distance %f\n", top_shifts[i],top_distances[i]);
			}
			else printf("First,enter a text!");
		}
	}
}
int main()
{
	menu();
}


//Short text : animal
//Metric 1 :
//	Shift 0 with a distance 98.340183
//	Shift 18 with a distance 98.342298
//	Shift 7 with a distance 98.350217
//
//	Metric 2 :
//	Shift 18 with a distance 0.448057
//	Shift 19 with a distance 0.494972
//	Shift 0 with a distance 0.496352
//
//	Metric 3 :
//	Shift 18 with a distance 25.365309
//	Shift 19 with a distance 25.387639
//	Shift 0 with a distance 25.388296
//
//	Fro short texts,metric 2 and metric 3 compute better(identic results)
//
//
//	Long texts : If he had anything confidential to say, he wrote it in cipher, that is, by so changing the order of the letters of the alphabet, that not a word could be made out.
//
//	chi - squared dst :
//Shift 0 with a distance 98.311170
//Shift 7 with a distance 98.339939
//Shift 14 with a distance 98.345900
//
//cosine dst :
//Shift 0 with a distance 0.037897
//Shift 11 with a distance 0.281164
//Shift 15 with a distance 0.357354
//
//eucld dst :
//Shift 0 with a distance 25.358934
//Shift 11 with a distance 25.426196
//Shift 15 with a distance 25.447225
//
//For long texts cos dst and eucld dst performs better(identic results)
// 
// 
//Short text = > the first metric selects the wrong shift from the 3 best possible shifts(2 are correct)
//Long text = > the first - metrci selects 2 wrong shifts from the 3 best possible(1 correct)

//If the text contains e ant t give better responses