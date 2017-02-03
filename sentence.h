#ifndef SENTENCE_H_81987F60
#define SENTENCE_H_81987F60

#include <verbly.h>
#include <random>
#include <string>

class sentence {
public:

  sentence(
    const verbly::database& database,
    std::mt19937& rng);

  std::string generate() const;

private:

  verbly::filter parseSelrestrs(verbly::selrestr selrestr) const;

  bool requiresSelrestr(std::string restriction, verbly::selrestr selrestr) const;

  verbly::word generateStandardNoun(std::string role, verbly::selrestr selrestrs) const;

  verbly::token generateStandardNounPhrase(
    const verbly::word& noun,
    std::string role,
    bool plural,
    bool definite) const;

  verbly::token generateClause(const verbly::token& it) const;

  void visit(verbly::token& it) const;

  const verbly::database& database_;
  std::mt19937& rng_;
};

#endif /* end of include guard: SENTENCE_H_81987F60 */
