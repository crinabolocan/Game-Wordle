#include <time.h>
#include "leaderboard.h"

#define MAX_NUM_OF_WORDS 500
#define CONST 500
char aux[5] = "-----";
char eliminate[30]="";
int processGuess(const char *theAnswer, const char *theGuess)
{
  // the clue
  char clue[6] = {'-', '-', '-', '-', '-', '\0'};

  // a set of flags indicating if that letter in the answer is used as clue
  int answerFlags[5] = {0, 0, 0, 0, 0};

  // first pass, look for exact matches
  for (int i = 0; i < 5; i++)
  {
    if (theGuess[i] == theAnswer[i])
    {
      clue[i] = 'G';
      answerFlags[i] = 1;
      aux[i] = theGuess[i];
    }
  }

  // second pass, look for there but elsewhere
  for (int i = 0; i < 5; i++)
  {
    if (clue[i] == '-')
    { // means no exact match from first pass
      int findletter = 0;
      for (int j = 0; j < 5; j++)
      {
        if (answerFlags[j] == 0 && theGuess[i] == theAnswer[j])
        {
          // a match at another position and has not been used as clue
          clue[i] = 'Y';
          answerFlags[j] = 1;
          findletter = 1;
          break; // end this j-loop because we don't want multiple clues from the same letter
        }
      }
      if (findletter == 0 && strchr(eliminate, theGuess[i]) == 0){

        strncat(eliminate, theGuess + i, 1);} 
    }
  }
  
  if (strcmp(clue, "GGGGG") != 0)
  {
    printf("Your hint: ");
    printf("%s\n", clue);
    printf("Word to find: %s\n", aux);
    printf("Try not to use: ");
    for(int i = 0; i<strlen(eliminate)-1; i++){
      printf("%c, ", eliminate[i]);
    }
    printf("%c\n", eliminate[strlen(eliminate)-1]);
     
  }
  if (strcmp(clue, "GGGGG") == 0) // if match strcmp returns 0
    return 1;
  return 0;
}

int main()
{

  // load the words
  char **wordsList = calloc(MAX_NUM_OF_WORDS, sizeof(char *));
  int wordCount = 0;
  char *fiveLetterWord = malloc(6 * sizeof(char));
  FILE *wordsFile = fopen("words.txt", "r");
  FILE *leaderboard = fopen("leaderboard.txt", "a");
  while (fscanf(wordsFile, "%s", fiveLetterWord) != EOF && wordCount < MAX_NUM_OF_WORDS)
  {
    wordsList[wordCount] = fiveLetterWord;
    wordCount++;
    fiveLetterWord = malloc(6 * sizeof(char));
  }
  fclose(wordsFile);

  // start the game
  // pick a word randomly
  srand(time(NULL));
  char *answer = wordsList[rand() % wordCount];

  // do the game loop
  int num_of_guesses = 0;
  int guessed_correctly = 0;
  char *guess = malloc(6 * sizeof(char));

  while (guessed_correctly == 0 && num_of_guesses < 5)
  {
    // get guess from player
    printf("Input a 5-letter word and press enter: ");
    scanf("%s", guess);
    if (strlen(guess) > 5 || strlen(guess) < 5)
      printf("Incorrect guess lenght!\n");
    else
    {
      int ok = 0;
      int count = 0;
      while (count < MAX_NUM_OF_WORDS && ok == 0)
      {
        if (strcmp(wordsList[count], guess) == 0)
        {
          ok = 1;
        }
        count++;
      }
      if (ok == 1)
      {
        printf("You have guessed %s\n", guess);
        num_of_guesses += 1;

        // process guess
        guessed_correctly = processGuess(answer, guess);
      }
      else
        printf("Your word is invalide!\n");
    }
  }

  // display end of game message
  if (guessed_correctly == 1)
  {
    printf("Congratulations! You guessed the correct word in %d times!\n", num_of_guesses);
    printf("Enter your name: ");
    char numePlayer[26];
    time_t t;
    time(&t);
    scanf("%s", numePlayer);
    fprintf(leaderboard, "%s %d %s", numePlayer, num_of_guesses, ctime(&t));
    fclose(leaderboard);
    
  }
  else
  {
    printf("You have used up yours guesses... the correct word is %s\n", answer);
  }
  printf("Want to see the leaderboard? (y/n)\n");
  char *command = malloc(2);
  scanf("%s", command);
  if(strcmp(command, "y")==0){
    printf("-------------------LeaderBoard-------------------\n");
    FILE * leaderboard2 = fopen("leaderboard.txt", "r");
    List *list = CreateList();
    char *line = malloc(CONST), *p;
    while(fgets(line, CONST, leaderboard2)){
      
      if(strchr(line, '\r')) p = strtok(line , "\r");
      if(strchr(line, '\n')) p = strtok(line , "\n");
      Player *p1 = malloc(sizeof(Player));
      p = strtok(line, " ");
      p1->nume = strdup(p);
      p = strtok(NULL, " ");
      p1->incercari = atoi(p);
      p = strtok(NULL, "\n");
      p1->date = strdup(p);
      int aux = FindInList(list, (void*)p1, cmp_lexic, cmp_incercari);
      if (aux != 0){
        AddList(list, p1, cmp_incercari);}
        else{
          free(p1->date);
          free(p1->nume);
          free(p1);
        }
    }
    Node *n = list->head;
    int i = 1;
    while(n){
      printf("%d. ", i);
      Print(n->value);
      n = n->next;
      i++;
    }
  fclose(leaderboard2);
  free(line);
  FreeList(&list, freePlayer);
  }

  for (int i = 0; i < wordCount; i++)
  {
    free(wordsList[i]);
  }
  free(command);
  free(wordsList);
  free(fiveLetterWord);
  free(guess);

  return 0;
}