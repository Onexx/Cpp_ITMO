#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

bool compare_ignore_case(const std::string &a, const std::string &b) {
  return std::lexicographical_compare(
      a.begin(), a.end(), b.begin(), b.end(),
      [](const unsigned char a, const unsigned char b) {
        return toupper(a) < toupper(b);
      });
}

size_t skip_begin(const std::string &s) {
  size_t idx = 0;
  while (idx < s.size() && (s[idx] == '0' || isspace(s[idx]))) {
    idx++;
  }
  return idx;
}

size_t skip_end(const std::string &s) {
  if (s.empty()) {
    return 0;
  }
  size_t idx = s.size() - 1;
  while (idx > 0 && isspace(s[idx])) {
    idx--;
  }
  return idx;
}

bool is_number(const std::string &s) {
  if (s.empty()) {
    return false;
  }
  return std::all_of(s.begin(), s.end(), [](auto i) {
    return isdigit(i) || isspace(i) || i == '-';
  });
}

bool only_digits(const std::string &s, size_t l, size_t r) {
  if (l >= r || r > s.size())
    return false;
  for (; l < r; l++) {
    if (!isdigit(s[l])) {
      return false;
    }
  }
  return true;
}

bool compare_numeric(const std::string &a, const std::string &b) {
  bool a_is_number = is_number(a);
  bool b_is_number = is_number(b);

  size_t a_begin = skip_begin(a);
  size_t b_begin = skip_begin(b);

  size_t a_end = skip_end(a);
  size_t b_end = skip_end(b);

  bool a_is_invalid = !a_is_number || a_end < a_begin;
  bool b_is_invalid = !b_is_number || b_end < b_begin;

  { // check invalid
    if (a_is_invalid && b_is_invalid) {
      return a < b;
    }
    if (b_is_invalid) {
      return a[a_begin] == '-';
    }
    if (a_is_invalid) {
      return b[b_begin] != '-';
    }
  }

  bool a_minus = false, b_minus = false;
  { // get minus
    if (a[a_begin] == '-') {
      a_minus = true;
      a_begin++;
    }
    if (b[b_begin] == '-') {
      b_minus = true;
      b_begin++;
    }
  }

  if (!only_digits(a, a_begin, a_end) && !only_digits(b, b_begin, b_end)) {
    return a < b;
  }

  // only digits left, comparing numbers
  if (a_minus != b_minus) {
    return a_minus;
  }

  size_t a_size = a_end - a_begin;
  size_t b_size = b_end - b_begin;

  if (a_size != b_size) {
    return a_size < b_size;
  }

  for (; a_begin < a_end; a_begin++, b_begin++) {
    if (a[a_begin] != b[b_begin]) {
      return a[a_begin] < b[b_begin];
    }
  }
  return false;
}

template <class C> void print_out(std::ostream &strm, const C &c) {
  std::ostream_iterator<typename C::value_type> out(strm, "\n");
  std::copy(c.begin(), c.end(), out);
}

void sort_stream(std::istream &input, const bool ignore_case,
                 const bool numeric_sort) {

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(input, line)) {
    lines.push_back(line);
  }

  if (numeric_sort) {
    std::sort(lines.begin(), lines.end(), compare_numeric);
  } else if (ignore_case) {
    std::sort(lines.begin(), lines.end(), compare_ignore_case);
  } else {
    std::sort(lines.begin(), lines.end());
  }

  print_out(std::cout, lines);
}

} // anonymous namespace

int main(int argc, char **argv) {
  bool ignore_case = false;
  bool numeric_sort = false;
  const char *input_name = nullptr;
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      if (argv[i][1] != '-') {
        const size_t len = std::strlen(argv[i]);
        for (size_t j = 1; j < len; ++j) {
          switch (argv[i][j]) {
          case 'f':
            ignore_case = true;
            break;
          case 'n':
            numeric_sort = true;
            break;
          }
        }
      } else {
        if (std::strcmp(argv[i], "--ignore-case") == 0) {
          ignore_case = true;
        } else if (std::strcmp(argv[i], "--numeric-sort") == 0) {
          numeric_sort = true;
        }
      }
    } else {
      input_name = argv[i];
    }
  }

  if (input_name != nullptr) {
    std::ifstream f(input_name);
    sort_stream(f, ignore_case, numeric_sort);
  } else {
    sort_stream(std::cin, ignore_case, numeric_sort);
  }
}
