#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void getOnePost(char* message, int num);

void getAllPosts(char* message);

void addPost(char* message, char* author, char* topic, char* title);

void updatePost(char* message, char* author, char* topic, char* title, int num);

void deletePost(char* message, int num);