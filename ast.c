void exit(int);

int getchar(void);

int main1();

int main()
{
    return main1();
}

void error()
{
    exit(1);
}

void be_push() {
    emit(1, "\x50");
}