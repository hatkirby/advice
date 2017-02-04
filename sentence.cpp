#include "sentence.h"
#include <algorithm>
#include <list>
#include <set>

sentence::sentence(
  const verbly::database& database,
  std::mt19937& rng) :
    database_(database),
    rng_(rng)
{
}

std::string sentence::generate() const
{
  // Generate the form that the title should take.
  verbly::token form;
  std::set<std::string> synrestrs {"infinitive_phrase", "bare", "subjectless"};
  std::set<std::string> secondSyn {"participle_phrase", "subjectless"};
  std::set<std::string> adjSyn {"adjective_phrase"};

  if (std::bernoulli_distribution(1.0/6.0)(rng_))
  {
    form << "not";
  }

  if (std::bernoulli_distribution(1.0/6.0)(rng_))
  {
    form << "be";
    form << adjSyn;
  } else {
    if (std::bernoulli_distribution(1.0/6.0)(rng_))
    {
      form << "get";
      synrestrs.insert("experiencer");
      synrestrs.insert("past_participle");
    }

    form << synrestrs;
  }

  if (std::bernoulli_distribution(1.0/5.0)(rng_))
  {
    if (std::bernoulli_distribution(1.0/4.0)(rng_))
    {
      form << "without";
    } else {
      form << "while";
    }

    form << secondSyn;
  }

  // Attempt to compile the form, restarting if a bad word is generated.
  std::set<std::string> badWords = {"raped"};

  verbly::token tok = form;
  std::list<std::string> words;
  for (;;)
  {
    // Compile the form.
    while (!tok.isComplete())
    {
      visit(tok);
    }

    std::string compiled = tok.compile();
    words = verbly::split<std::list<std::string>>(compiled, " ");

    // Ensure that there are no bad words in the output.
    if (!std::any_of(std::begin(words), std::end(words), [&badWords] (const std::string& word) {
      std::string canonWord;

      for (char ch : word)
      {
        if (std::isalpha(ch))
        {
          canonWord.push_back(std::tolower(ch));
        }
      }

      return (badWords.count(canonWord) == 1);
    })) {
      break;
    } else {
      std::cout << "Bad word generated." << std::endl;
    }
  }

  // Put the form into title case.
  for (std::string& word : words)
  {
    if ((word[0] == '"') && (word.length() > 1))
    {
      word[1] = std::toupper(word[1]);
    } else {
      word[0] = std::toupper(word[0]);
    }
  }

  return verbly::implode(std::begin(words), std::end(words), " ");
}

verbly::filter sentence::parseSelrestrs(
  verbly::selrestr selrestr) const
{
  switch (selrestr.getType())
  {
    case verbly::selrestr::type::empty:
    {
      return {};
    }

    case verbly::selrestr::type::singleton:
    {
      verbly::filter result;

      if (selrestr.getRestriction() == "concrete")
      {
        result = (verbly::notion::wnid == 100001930); // physical entity
      } else if (selrestr.getRestriction() == "time")
      {
        result = (verbly::notion::wnid == 100028270); // time
      } else if (selrestr.getRestriction() == "state")
      {
        result = (verbly::notion::wnid == 100024720); // state
      } else if (selrestr.getRestriction() == "abstract")
      {
        result = (verbly::notion::wnid == 100002137); // abstract entity
      } else if (selrestr.getRestriction() == "scalar")
      {
        result = (verbly::notion::wnid == 103835412); // number
      } else if (selrestr.getRestriction() == "currency")
      {
        result = (verbly::notion::wnid == 105050379); // currency
      } else if (selrestr.getRestriction() == "location")
      {
        result = (verbly::notion::wnid == 100027167); // location
      } else if (selrestr.getRestriction() == "organization")
      {
        result = (verbly::notion::wnid == 100237078); // organization
      } else if (selrestr.getRestriction() == "int_control")
      {
        result = (verbly::notion::wnid == 100007347); // causal agent
      } else if (selrestr.getRestriction() == "natural")
      {
        result = (verbly::notion::wnid == 100019128); // natural object
      } else if (selrestr.getRestriction() == "phys_obj")
      {
        result = (verbly::notion::wnid == 100002684); // physical object
      } else if (selrestr.getRestriction() == "solid")
      {
        result = (verbly::notion::wnid == 113860793); // solid
      } else if (selrestr.getRestriction() == "shape")
      {
        result = (verbly::notion::wnid == 100027807); // shape
      } else if (selrestr.getRestriction() == "substance")
      {
        result = (verbly::notion::wnid == 100019613); // substance
      } else if (selrestr.getRestriction() == "idea")
      {
        result = (verbly::notion::wnid == 105803379); // idea
      } else if (selrestr.getRestriction() == "sound")
      {
        result = (verbly::notion::wnid == 107111047); // sound
      } else if (selrestr.getRestriction() == "communication")
      {
        result = (verbly::notion::wnid == 100033020); // communication
      } else if (selrestr.getRestriction() == "region")
      {
        result = (verbly::notion::wnid == 105221895); // region
      } else if (selrestr.getRestriction() == "place")
      {
        result = (verbly::notion::wnid == 100586262); // place
      } else if (selrestr.getRestriction() == "machine")
      {
        result = (verbly::notion::wnid == 102958343); // machine
      } else if (selrestr.getRestriction() == "animate")
      {
        result = (verbly::notion::wnid == 100004258); // animate thing
      } else if (selrestr.getRestriction() == "plant")
      {
        result = (verbly::notion::wnid == 103956922); // plant
      } else if (selrestr.getRestriction() == "comestible")
      {
        result = (verbly::notion::wnid == 100021265); // food
      } else if (selrestr.getRestriction() == "artifact")
      {
        result = (verbly::notion::wnid == 100021939); // artifact
      } else if (selrestr.getRestriction() == "vehicle")
      {
        result = (verbly::notion::wnid == 104524313); // vehicle
      } else if (selrestr.getRestriction() == "human")
      {
        result = (verbly::notion::wnid == 100007846); // person
      } else if (selrestr.getRestriction() == "animal")
      {
        result = (verbly::notion::wnid == 100015388); // animal
      } else if (selrestr.getRestriction() == "body_part")
      {
        result = (verbly::notion::wnid == 105220461); // body part
      } else if (selrestr.getRestriction() == "garment")
      {
        result = (verbly::notion::wnid == 103051540); // clothing
      } else if (selrestr.getRestriction() == "tool")
      {
        result = (verbly::notion::wnid == 104451818); // tool
      } else {
        return {};
      }

      std::cout << selrestr.getRestriction() << " (" << selrestr.getPos() << ")" << std::endl;

      if (selrestr.getPos())
      {
        return (verbly::notion::fullHypernyms %= result);
      } else {
        return !(verbly::notion::fullHypernyms %= result);
      }
    }

    case verbly::selrestr::type::group:
    {
      std::cout << "or: " << selrestr.getOrlogic() << std::endl;
      verbly::filter ret(selrestr.getOrlogic());

      for (const verbly::selrestr& child : selrestr)
      {
        ret += parseSelrestrs(child);
      }

      return ret;
    }
  }
}

bool sentence::requiresSelrestr(
  std::string restriction,
  verbly::selrestr selrestr) const
{
  switch (selrestr.getType())
  {
    case verbly::selrestr::type::empty:
    {
      return false;
    }

    case verbly::selrestr::type::singleton:
    {
      if (selrestr.getRestriction() == restriction)
      {
        return selrestr.getPos();
      } else {
        return false;
      }
    }

    case verbly::selrestr::type::group:
    {
      if (selrestr.getOrlogic())
      {
        return std::all_of(std::begin(selrestr), std::end(selrestr), [=] (const verbly::selrestr& s) {
          return requiresSelrestr(restriction, s);
        });
      } else {
        return std::any_of(std::begin(selrestr), std::end(selrestr), [=] (const verbly::selrestr& s) {
          return requiresSelrestr(restriction, s);
        });
      }
    }
  }
}

verbly::word sentence::generateStandardNoun(
  std::string role,
  verbly::selrestr selrestrs) const
{
  std::geometric_distribution<int> tagdist(0.5); // 0.06
  std::vector<verbly::word> result;
  bool trySelection = true;

  while (result.empty())
  {
    verbly::filter condition =
      (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
      && (verbly::form::proper == false)
      //&& (verbly::form::complexity == 1)
     // && (verbly::word::tagCount >= tagdist(rng_)) // Favor more common words
      && (verbly::word::tagCount >= 1)
      && !(verbly::word::usageDomains %= (verbly::notion::wnid == 106718862)); // Blacklist ethnic slurs

    // Only use selection restrictions for a first attempt.
    if (trySelection)
    {
      verbly::filter selrestrCondition = parseSelrestrs(selrestrs).compact();

      if (selrestrCondition.getType() != verbly::filter::type::empty)
      {
        condition &= std::move(selrestrCondition);
      } else if (role == "Attribute")
      {
        condition &= (verbly::notion::fullHypernyms %= (verbly::notion::wnid == 100024264)); // attribute
      } else if (role == "Instrument")
      {
        condition &= (verbly::notion::fullHypernyms %= (verbly::notion::wnid == 104451818)); // tool
      } else if (role == "Agent")
      {
        condition &= (verbly::notion::fullHypernyms %= (verbly::notion::wnid == 100007347)); // causal agent
      }

      trySelection = false;
    } else {
      std::cout << "Selection failed" << std::endl;
    }

    result = database_.words(condition).all();
  }

  return result.front();
}

verbly::token sentence::generateStandardNounPhrase(
  const verbly::word& noun,
  std::string role,
  bool plural,
  bool definite) const
{
  verbly::token utter;
  verbly::word sounder = noun;
  verbly::word descript;

  if (std::bernoulli_distribution(1.0/8.0)(rng_))
  {
    std::geometric_distribution<int> tagdist(0.2);
    descript = database_.words(
      (verbly::word::tagCount >= tagdist(rng_))
      && (verbly::notion::partOfSpeech == verbly::part_of_speech::adjective)).first();

    sounder = descript;
  }

  if ((std::bernoulli_distribution(1.0/3.0)(rng_)) && (definite))
  {
    utter << "the";

    if (std::bernoulli_distribution(1.0/2.0)(rng_))
    {
      plural = true;
    }
  } else {
    if ((role != "Theme") && (role != "Attribute") && std::bernoulli_distribution(1.0/2.0)(rng_))
    {
      utter << "your";
    } else if (!plural) {
      if (sounder.getLemma().getBaseForm().startsWithVowelSound())
      {
        utter << "an";
      } else {
        utter << "a";
      }
    }
  }

  if (descript)
  {
    utter << descript;
  }

  if (plural && noun.getLemma().hasInflection(verbly::inflection::plural))
  {
    utter << verbly::token(noun, verbly::inflection::plural);
  } else {
    utter << noun;
  }

  return utter;
}

verbly::token sentence::generateClause(
  const verbly::token& it) const
{
  verbly::token utter;
  std::geometric_distribution<int> tagdist(0.07);
  std::vector<verbly::word> verbDataset;

  verbly::filter frameCondition =
    (verbly::frame::length >= 2)
    && (verbly::frame::parts(0) %= (
      (verbly::part::type == verbly::part_type::noun_phrase)
      && (verbly::part::role == "Agent"))
    && (verbly::frame::parts(1) %=
      (verbly::part::type == verbly::part_type::verb))
    && !(verbly::frame::parts() %= (
      verbly::part::synrestrs %= "adjp")));

  if (it.hasSynrestr("experiencer"))
  {
    frameCondition &=
      (verbly::frame::parts(2) %=
        (verbly::part::type == verbly::part_type::noun_phrase)
        && !(verbly::part::synrestrs %= "genitive")
        && ((verbly::part::role == "Patient")
          || (verbly::part::role == "Experiencer")));
  }

  verbly::filter verbCondition =
    (verbly::notion::partOfSpeech == verbly::part_of_speech::verb)
    && frameCondition;

  if (it.hasSynrestr("participle_phrase"))
  {
    verbCondition &= (verbly::lemma::forms(verbly::inflection::ing_form));
  } else if (it.hasSynrestr("progressive"))
  {
    verbCondition &= (verbly::lemma::forms(verbly::inflection::s_form));
  } else if (it.hasSynrestr("past_participle"))
  {
    verbCondition &= (verbly::lemma::forms(verbly::inflection::past_participle));
  }

  // Because of the tag distribution, it's possible (albeit extremely unlikely)
  // for the verb query to fail, so we loop until it succeeds.
  while (verbDataset.empty())
  {
    verbDataset = database_.words(
      verbCondition
      && (verbly::word::tagCount >= tagdist(rng_))
    ).all();
  }

  verbly::word verb = verbDataset.front();
  verbly::frame frame = database_.frames(frameCondition && verb).first();
  std::list<verbly::part> parts(std::begin(frame.getParts()), std::end(frame.getParts()));

  if (it.hasSynrestr("experiencer"))
  {
    // Ignore the direct object.
    parts.erase(std::next(parts.begin(), 2));
  }

  if (it.hasSynrestr("subjectless"))
  {
    // Ignore the subject.
    parts.pop_front();
  }

  for (const verbly::part& part : parts)
  {
    switch (part.getType())
    {
      case verbly::part_type::noun_phrase:
      {
        std::cout << "NP: ";
        for (auto& s : part.getNounSynrestrs())
        {
          std::cout << s << " ";
        }
        std::cout << std::endl;

        if (requiresSelrestr("currency", part.getNounSelrestrs()))
        {
          int lead = std::uniform_int_distribution<int>(1,9)(rng_);
          int tail = std::uniform_int_distribution<int>(0,6)(rng_);
          std::string tailStr(tail, '0');

          utter << ("$" + std::to_string(lead) + tailStr);
        } else if (part.nounHasSynrestr("adjp"))
        {
          utter << std::set<std::string>({"adjective_phrase"});
        } else if ((part.nounHasSynrestr("be_sc_ing"))
          || (part.nounHasSynrestr("ac_ing"))
          || (part.nounHasSynrestr("sc_ing"))
          || (part.nounHasSynrestr("np_omit_ing"))
          || (part.nounHasSynrestr("oc_ing")))
        {
          utter << std::set<std::string>({"participle_phrase", "subjectless"});
        } else if ((part.nounHasSynrestr("poss_ing"))
          || (part.nounHasSynrestr("possing"))
          || (part.nounHasSynrestr("pos_ing")))
        {
          utter << "your";
          utter << std::set<std::string>({"participle_phrase", "subjectless"});
        } else if (part.nounHasSynrestr("genitive"))
        {
          utter << "your";
        } else if (part.nounHasSynrestr("adv_loc"))
        {
          if (std::bernoulli_distribution(1.0/2.0)(rng_))
          {
            utter << "here";
          } else {
            utter << "there";
          }
        } else if (part.nounHasSynrestr("refl"))
        {
          utter << "yourself";
        } else if ((part.nounHasSynrestr("sc_to_inf"))
          || (part.nounHasSynrestr("ac_to_inf"))
          || (part.nounHasSynrestr("vc_to_inf"))
          || (part.nounHasSynrestr("rs_to_inf"))
          || (part.nounHasSynrestr("oc_to_inf")))
        {
          utter << std::set<std::string>({"infinitive_phrase", "subjectless"});
        } else if (part.nounHasSynrestr("oc_bare_inf"))
        {
          utter << std::set<std::string>({"infinitive_phrase", "bare", "subjectless"});
        } else if (part.nounHasSynrestr("wh_comp"))
        {
          utter << "whether";

          verbly::token sentence(std::set<std::string>({"progressive"}));
          utter << generateClause(sentence);
        } else if (part.nounHasSynrestr("that_comp"))
        {
          utter << "that";
          utter << "they";

          verbly::token sentence(std::set<std::string>({"subjectless"}));
          utter << generateClause(sentence);
        } else if (part.nounHasSynrestr("what_extract"))
        {
          utter << "what";

          verbly::token sentence(std::set<std::string>({"progressive", "experiencer"}));
          utter << generateClause(sentence);
        } else if (part.nounHasSynrestr("how_extract"))
        {
          utter << "how";

          verbly::token sentence(std::set<std::string>({"progressive"}));
          utter << generateClause(sentence);
        } else if (part.nounHasSynrestr("wh_inf"))
        {
          utter << "how";

          verbly::token sentence(std::set<std::string>({"infinitive_phrase", "subjectless"}));
          utter << generateClause(sentence);
        } else if (part.nounHasSynrestr("what_inf"))
        {
          utter << "what";

          verbly::token sentence(std::set<std::string>({"infinitive_phrase", "subjectless", "experiencer"}));
          utter << generateClause(sentence);
        } else if (part.nounHasSynrestr("wheth_inf"))
        {
          utter << "whether";

          verbly::token sentence(std::set<std::string>({"infinitive_phrase", "subjectless"}));
          utter << generateClause(sentence);
        } else if (part.nounHasSynrestr("quotation"))
        {
          verbly::token sentence(std::set<std::string>({"participle_phrase"}));
          while (!sentence.isComplete())
          {
            visit(sentence);
          }

          utter << ("\"" + sentence.compile() + "\"");
        } else {
          verbly::word noun = generateStandardNoun(part.getNounRole(), part.getNounSelrestrs());

          bool plural = part.nounHasSynrestr("plural");
          if (!plural)
          {
            plural = requiresSelrestr("plural", part.getNounSelrestrs());
          }

          utter << generateStandardNounPhrase(
            noun,
            part.getNounRole(),
            plural,
            part.nounHasSynrestr("definite"));

          if (part.nounHasSynrestr("acc_ing") || part.nounHasSynrestr("ac_ing"))
          {
            utter << std::set<std::string>({"participle_phrase", "subjectless"});
          }
        }

        break;
      }

      case verbly::part_type::verb:
      {
        std::cout << "V: " << verb.getBaseForm() << std::endl;

        if (it.hasSynrestr("progressive"))
        {
          utter << verbly::token(verb, verbly::inflection::s_form);
        } else if (it.hasSynrestr("past_participle"))
        {
          utter << verbly::token(verb, verbly::inflection::past_participle);
        } else if (it.hasSynrestr("infinitive_phrase"))
        {
          if (!it.hasSynrestr("bare"))
          {
            utter << "to";
          }

          utter << verb;
        } else if (it.hasSynrestr("participle_phrase"))
        {
          utter << verbly::token(verb, verbly::inflection::ing_form);
        } else {
          utter << verb;
        }

        break;
      }

      case verbly::part_type::preposition:
      {
        std::cout << "PREP" << std::endl;

        if (part.isPrepositionLiteral())
        {
          int choiceIndex = std::uniform_int_distribution<int>(0, part.getPrepositionChoices().size()-1)(rng_);
          utter << part.getPrepositionChoices()[choiceIndex];
        } else {
          verbly::filter pgf(true);
          for (const std::string& choice : part.getPrepositionChoices())
          {
            pgf += (verbly::notion::prepositionGroups == choice);
          }

          utter << database_.words(pgf && (verbly::notion::partOfSpeech == verbly::part_of_speech::preposition)).first();
        }

        break;
      }

      case verbly::part_type::adjective:
      {
        std::cout << "ADJ" << std::endl;

        utter << std::set<std::string>({"adjective_phrase"});

        break;
      }

      case verbly::part_type::adverb:
      {
        std::cout << "ADV" << std::endl;

        utter << std::set<std::string>({"adverb_phrase"});

        break;
      }

      case verbly::part_type::literal:
      {
        std::cout << "LIT" << std::endl;

        utter << part.getLiteralValue();

        break;
      }

      case verbly::part_type::invalid:
      {
        // Nope

        break;
      }
    }
  }

  if ((parts.size() == 1) && (std::bernoulli_distribution(1.0/4.0)(rng_)))
  {
    utter << std::set<std::string>({"adverb_phrase"});
  }

  return utter;
}

void sentence::visit(verbly::token& it) const
{
  switch (it.getType())
  {
    case verbly::token::type::utterance:
    {
      for (verbly::token& token : it)
      {
        if (!token.isComplete())
        {
          visit(token);

          break;
        }
      }

      break;
    }

    case verbly::token::type::fillin:
    {
      if (it.hasSynrestr("infinitive_phrase"))
      {
        it = generateClause(it);
      } else if (it.hasSynrestr("adjective_phrase"))
      {
        verbly::token phrase;

        if (std::bernoulli_distribution(1.0/6.0)(rng_))
        {
          phrase << std::set<std::string>({"adverb_phrase"});
        }

        if (std::bernoulli_distribution(1.0/4.0)(rng_))
        {
          phrase << std::set<std::string>({"participle_phrase", "subjectless"});
        } else {
          std::geometric_distribution<int> tagdist(0.2);
          phrase << database_.words(
            (verbly::word::tagCount >= tagdist(rng_))
            && (verbly::notion::partOfSpeech == verbly::part_of_speech::adjective)).first();
        }

        it = phrase;
      } else if (it.hasSynrestr("adverb_phrase"))
      {
        std::geometric_distribution<int> tagdist(1.0/23.0);

        it = database_.words(
          (verbly::notion::partOfSpeech == verbly::part_of_speech::adverb)
          && (verbly::word::tagCount >= tagdist(rng_))
          ).first();
      } else if (it.hasSynrestr("participle_phrase"))
      {
        if (std::bernoulli_distribution(1.0/2.0)(rng_))
        {
          it = verbly::token(
            database_.words(
              (verbly::notion::partOfSpeech == verbly::part_of_speech::verb)
              && (verbly::lemma::forms(verbly::inflection::ing_form))).first(),
            verbly::inflection::ing_form);
        } else {
          it = generateClause(it);
        }
      } else {
        it = "*the reality of the situation*";
      }

      break;
    }

    case verbly::token::type::word:
    case verbly::token::type::literal:
    case verbly::token::type::part:
    {
      // Nope

      break;
    }
  }
}
