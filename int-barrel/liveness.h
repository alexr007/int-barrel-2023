char liveness = '-';

char next(char curr) {
  switch (curr) {
    case '-': return '/';
    case '/': return '|';
    case '|': return '-';
    default: return '-';
  }
}

String liveness_char() {
  liveness = next(liveness);
  char s[2];
  s[0] = liveness;
  s[1] = 0;
  return s;
}
