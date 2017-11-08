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
  verbly::filter blacklist;

  for (std::string word : {
    "raped", "Negro"
  })
  {
    blacklist |= (verbly::form::text == word);
  }

  badWords_ = !blacklist;

   // Blacklist ethnic slurs
  badWords_ &= !(verbly::word::usageDomains %= (verbly::notion::wnid == 106718862));
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

  // Compile the form.
  verbly::token tok = verbly::token::capitalize(
    verbly::token::casing::title_case, form);

  while (!tok.isComplete())
  {
    visit(tok);
  }

  std::string compiled = tok.compile();

  return compiled;
}

bool sentence::chooseSelrestr(std::set<std::string> selrestrs, std::set<std::string> choices) const
{
  int validChoices = 0;
  for (const std::string& choice : choices)
  {
    if (selrestrs.count(choice))
    {
      validChoices++;
    }
  }

  return std::bernoulli_distribution(static_cast<double>(validChoices)/static_cast<double>(selrestrs.size()))(rng_);
}

verbly::word sentence::generateStandardNoun(
  std::string role,
  std::set<std::string> selrestrs) const
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
      && badWords_;

    // Only use selection restrictions for a first attempt.
    if (trySelection)
    {
      verbly::filter selection(true);

      for (const std::string& selrestr : selrestrs)
      {
        if (selrestr == "concrete")
        {
          selection += (verbly::notion::wnid == 100001930); // physical entity
        } else if (selrestr == "time")
        {
          selection += (verbly::notion::wnid == 100028270); // time
        } else if (selrestr == "state")
        {
          selection += (verbly::notion::wnid == 100024720); // state
        } else if (selrestr == "abstract")
        {
          selection += (verbly::notion::wnid == 100002137); // abstract entity
        } else if (selrestr == "scalar")
        {
          selection += (verbly::notion::wnid == 103835412); // number
        } else if (selrestr == "currency")
        {
          selection += (verbly::notion::wnid == 105050379); // currency
        } else if (selrestr == "location")
        {
          selection += (verbly::notion::wnid == 100027167); // location
        } else if (selrestr == "organization")
        {
          selection += (verbly::notion::wnid == 100237078); // organization
        } else if (selrestr == "int_control")
        {
          selection += (verbly::notion::wnid == 100007347); // causal agent
        } else if (selrestr == "natural")
        {
          selection += (verbly::notion::wnid == 100019128); // natural object
        } else if (selrestr == "phys_obj")
        {
          selection += (verbly::notion::wnid == 100002684); // physical object
        } else if (selrestr == "solid")
        {
          selection += (verbly::notion::wnid == 113860793); // solid
        } else if (selrestr == "shape")
        {
          selection += (verbly::notion::wnid == 100027807); // shape
        } else if (selrestr == "substance")
        {
          selection += (verbly::notion::wnid == 100019613); // substance
        } else if (selrestr == "idea")
        {
          selection += (verbly::notion::wnid == 105803379); // idea
        } else if (selrestr == "sound")
        {
          selection += (verbly::notion::wnid == 107111047); // sound
        } else if (selrestr == "communication")
        {
          selection += (verbly::notion::wnid == 100033020); // communication
        } else if (selrestr == "region")
        {
          selection += (verbly::notion::wnid == 105221895); // region
        } else if (selrestr == "place")
        {
          selection += (verbly::notion::wnid == 100586262); // place
        } else if (selrestr == "machine")
        {
          selection += (verbly::notion::wnid == 102958343); // machine
        } else if (selrestr == "animate")
        {
          selection += (verbly::notion::wnid == 100004258); // animate thing
        } else if (selrestr == "plant")
        {
          selection += (verbly::notion::wnid == 103956922); // plant
        } else if (selrestr == "comestible")
        {
          selection += (verbly::notion::wnid == 100021265); // food
        } else if (selrestr == "artifact")
        {
          selection += (verbly::notion::wnid == 100021939); // artifact
        } else if (selrestr == "vehicle")
        {
          selection += (verbly::notion::wnid == 104524313); // vehicle
        } else if (selrestr == "human")
        {
          selection += (verbly::notion::wnid == 100007846); // person
        } else if (selrestr == "animal")
        {
          selection += (verbly::notion::wnid == 100015388); // animal
        } else if (selrestr == "body_part")
        {
          selection += (verbly::notion::wnid == 105220461); // body part
        } else if (selrestr == "garment")
        {
          selection += (verbly::notion::wnid == 103051540); // clothing
        } else if (selrestr == "tool")
        {
          selection += (verbly::notion::wnid == 104451818); // tool
        } else if ((selrestr == "concrete_inanimate") || (selrestr == "inanimate"))
        {
          selection += (verbly::notion::wnid == 100021939); // artifact
          selection += (verbly::notion::wnid == 100019128); // natural object
        } else if (selrestr == "non_region_location")
        {
          selection += (verbly::notion::wnid == 102913152); // building
        } else if (selrestr == "non_solid_food")
        {
          selection += (verbly::notion::wnid == 107881800); // beverage
        } else if (selrestr == "solid_food")
        {
          selection += (verbly::notion::wnid == 107555863); // solid food
        } else if (selrestr == "slinky")
        {
          selection += (verbly::notion::wnid == 103670849); // line
        }
      }

      if (selection.compact().getType() != verbly::filter::type::empty)
      {
        condition &= (verbly::notion::fullHypernyms %= std::move(selection));
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
  bool indefiniteArticle = false;

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
      indefiniteArticle = true;
    }
  }

  if (std::bernoulli_distribution(1.0/8.0)(rng_))
  {
    std::geometric_distribution<int> tagdist(0.2);

    utter << database_.words(
      (verbly::word::tagCount >= tagdist(rng_))
      && (verbly::notion::partOfSpeech == verbly::part_of_speech::adjective)
      && badWords_).first();
  }

  if (plural && noun.hasInflection(verbly::inflection::plural))
  {
    utter << verbly::token(noun, verbly::inflection::plural);
  } else {
    utter << noun;
  }

  if (indefiniteArticle)
  {
    return verbly::token::indefiniteArticle(utter);
  } else {
    return utter;
  }
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
    && frameCondition
    && badWords_;

  if (it.hasSynrestr("participle_phrase"))
  {
    verbCondition &= (verbly::word::forms(verbly::inflection::ing_form));
  } else if (it.hasSynrestr("progressive"))
  {
    verbCondition &= (verbly::word::forms(verbly::inflection::s_form));
  } else if (it.hasSynrestr("past_participle"))
  {
    verbCondition &= (verbly::word::forms(verbly::inflection::past_participle));
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

        if (chooseSelrestr(part.getNounSelrestrs(), {"currency"}))
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
          utter << verbly::token::quote("\"", "\"",
            verbly::token(std::set<std::string>({"past_participle"})));
        } else {
          if (part.nounHasSynrestr("genitive"))
          {
            verbly::word noun = generateStandardNoun("Passive", {"animate"});
            verbly::token owner = generateStandardNounPhrase(noun, "Passive", false, true);

            utter << verbly::token::punctuation("'s", owner);
          }

          verbly::word noun = generateStandardNoun(part.getNounRole(), part.getNounSelrestrs());

          bool plural = part.nounHasSynrestr("plural") || chooseSelrestr(part.getNounSelrestrs(), {"group", "plural"});

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
        std::cout << "V: " << verb.getBaseForm().getText() << std::endl;

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
            && (verbly::notion::partOfSpeech == verbly::part_of_speech::adjective)
            && badWords_).first();
        }

        it = phrase;
      } else if (it.hasSynrestr("adverb_phrase"))
      {
        std::geometric_distribution<int> tagdist(1.0/23.0);

        it = database_.words(
          (verbly::notion::partOfSpeech == verbly::part_of_speech::adverb)
          && (verbly::word::tagCount >= tagdist(rng_))
          && badWords_).first();
      } else if (it.hasSynrestr("participle_phrase"))
      {
        if (std::bernoulli_distribution(1.0/2.0)(rng_))
        {
          it = verbly::token(
            database_.words(
              (verbly::notion::partOfSpeech == verbly::part_of_speech::verb)
              && (verbly::word::forms(verbly::inflection::ing_form))
              && badWords_).first(),
            verbly::inflection::ing_form);
        } else {
          it = generateClause(it);
        }
      } else if (it.hasSynrestr("past_participle"))
      {
        it = generateClause(it);
      } else {
        it = "*the reality of the situation*";
      }

      break;
    }

    case verbly::token::type::transform:
    {
      visit(it.getInnerToken());

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
