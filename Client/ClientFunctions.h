#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void getOnePost(char* message, int num);

void getAllPosts(char* message);

void getFilter(char* message, char* author, char* topic, char* title);

void addPost(char* message, char* author, char* topic, char* title);

void updatePost(char* message, char* author, char* topic, char* title, int num);

void deletePost(char* message, int num);


void exitMenuandCall(char* message);

void getSinglePostInputandCall(char* message);

void postInputandCall(char* message);

void putInputandCall(char* message);

void deleteInputandCall(char* message);