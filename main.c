#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <raylib.h>

#define FILENAME "image.ppm"

typedef struct ParsedImage
{
	unsigned int width;
	unsigned int height;
	Color *pixels;
} ParsedImage;

// returns one header line or NULL (EOF)
// skips comment lines
char* read_ppm_line(FILE *fp, char *buffer, size_t size)
{
	char* result;
	while ((result = fgets(buffer, size, fp)) != NULL) {
		if (result[0] != '#') {
			return result;
		}
	}

	return NULL; // EOF
}

char* expect_ppm_line(FILE *fp, char *buffer, size_t size) {
	char* line = read_ppm_line(fp, buffer, size);
	if (line == NULL) {
		printf("Unexpected EOF.");
	}

	return line;
}


ParsedImage *read_image()
{
	FILE *fp = fopen(FILENAME, "rb");

	if (fp == NULL)
	{
		printf("Failed to open file.");
		return NULL;
	}

	// PPM P6 (binary) Format

	// P6\n						 	# P3 (ASCII) or P6 (binary), we only support binary
	// 640 480\n 					# Width Height
	// 255\n						# Max Color Value, we only support 255
	// <binary RGB data>			# Raw bytes: RGBRGBRGB... (pixelCount * 3 bytes)

	char throwaway[1000]; // throwaway buffer

	// Parse Format

	char* line = expect_ppm_line(fp, throwaway, sizeof(throwaway));
	if (line == NULL) {
		fclose(fp);
		return NULL;
	}

	if (strcmp(line, "P6\n") != 0)
	{
		printf("Invalid file format: %s\n", line);
		fclose(fp);
		return NULL;
	}

	// Parse Dimensions
	line = expect_ppm_line(fp, throwaway, sizeof(throwaway));
	if (line == NULL) {
		fclose(fp);
		return NULL;
	}

	unsigned int width, height;
	// we should get two elements back from sscanf
	if (sscanf(line, "%u %u", &width, &height) != 2) { 
		printf("Failed to parse dimensions\n");
		fclose(fp);
		return NULL;
	}

	// Parse Max Color Value
	line = expect_ppm_line(fp, throwaway, sizeof(throwaway));
	if (line == NULL) {
		fclose(fp);
		return NULL;
	}

	int maxColors;
	if (sscanf(line, "%d", &maxColors) != 1 || maxColors != 255) {
		printf("Expected 255 for max colors, got: %s\n", line);
		fclose(fp);
		return NULL;
	}

	// Parse Raw Data

	int pixelCount = width * height;

	Color* pixels = malloc(sizeof(Color) * pixelCount);
	if (pixels == NULL) {
		printf("failed to allocate memory\n");
		fclose(fp);
		return NULL;
	}

	// xxx(mo): reading the whole file is faster
	for (int i = 0; i < pixelCount; i++) {
		unsigned char  rgb[3];
		if (fread(rgb, 1, 3, fp) != 3) {
			printf("Failed to read pixel data\n");
			free(pixels);
			fclose(fp);
			return NULL;
		}

		pixels[i] = (Color){rgb[0], rgb[1], rgb[2], 255}; // RGBA
	}

	fclose(fp);

	ParsedImage *img = malloc(sizeof(ParsedImage));
	if (img == NULL) {
		printf("Failed to allocate image\n");
		free(pixels);
		return NULL;
	}

	img->height = height;
	img->width = width;
	img->pixels = pixels;

	return img;
}

void free_image(ParsedImage *img)
{
	free(img->pixels);
	free(img);
}

int main(void)
{
	ParsedImage *img = read_image();
	if (img == NULL) {
		printf("waaaaaa\n");
		return 1;
	}

	// Window setup
	const int screenWidth = img->width; // for now, we wont do any scaling!
	const int screenHeight = img->height;

	InitWindow(screenWidth, screenHeight, "Image Viewer");
	SetTargetFPS(60);

	// Main loop
	while (!WindowShouldClose())
	{
		// Update

		// Draw
		BeginDrawing();
		ClearBackground(RAYWHITE);

		for (int x = 0; x < img->width; x++) {
			for (int y = 0; y < img->height; y++) {
				Color pixel = img->pixels[(y * img->width) + x];
				DrawPixel(x, y, pixel);
			}
		}

		EndDrawing();
	}

	CloseWindow();
	// xxx(mo): we actually shouldnt free this (since the app is exiting anyways!).
	// but i added this just to practice writing free functions for structs!
	free_image(img);
	return 0;
}
