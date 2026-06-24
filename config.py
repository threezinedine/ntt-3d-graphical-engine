import os
import json
import logging
import argparse
import colorlog
from jinja2 import Template
from dacite import from_dict
from dataclasses import dataclass, field

logger = logging.getLogger("CONFIG")
handler = colorlog.StreamHandler()

formatter = colorlog.ColoredFormatter(
    "%(log_color)s[%(name)-7s] %(message)s",
    log_colors={
        "DEBUG": "cyan",
        "INFO": "green",
        "WARNING": "yellow",
        "ERROR": "red",
        "CRITICAL": "bold_red",
    },
)

handler.setFormatter(formatter)
logger.addHandler(handler)


@dataclass
class Profile:
    inherit: list[str] = field(default_factory=list)
    options: list[str] = field(default_factory=list)
    command: str | None = None


def load_options(profile: Profile, profiles: dict[str, Profile]) -> dict[str, bool]:
    """
    The options will have the format NTT_DEBUG=1, -> {"NTT_DEBUG": True}
    """
    options = {}

    if profile.inherit:
        for parent_profile_name in profile.inherit:
            if parent_profile_name not in profiles:
                logger.error(
                    f"Profile '{parent_profile_name}' not found for inheritance."
                )
                exit(1)
            parent_profile = profiles[parent_profile_name]
            parent_options = load_options(parent_profile, profiles)
            options.update(parent_options)

    for option in profile.options:
        if "=" in option:
            key, value = option.split("=", 1)
            options[key] = value == "1"
        else:
            logger.error(f"Invalid option format: {option}")
            exit(1)
    return options


def main():
    parser = argparse.ArgumentParser(description="Load and process profiles.")
    parser.add_argument(
        "-p", "--profile", type=str, default="debug", help="Profile name to load"
    )
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Enable verbose logging"
    )
    args = parser.parse_args()

    if args.verbose:
        logger.setLevel(logging.DEBUG)
    else:
        logger.setLevel(logging.INFO)

    profiles: dict[str, Profile] = {}

    with open("profiles.json", "r") as f:
        profiles_dict = json.load(f)

    for name, profile_dict in profiles_dict.items():
        profile = from_dict(Profile, profile_dict)
        profiles[name] = profile

    logger.debug(f"Loaded profiles: {list(profiles.keys())}")

    if args.profile not in profiles:
        logger.error(f"Profile '{args.profile}' not found.")
        exit(1)

    if profiles[args.profile].command is None:
        logger.error(f"Profile '{args.profile}' does not have a command defined.")
        exit(1)

    logger.info(f"Using profile: {args.profile}")

    options = load_options(profiles[args.profile], profiles)
    options_str = " ".join(
        f"-D{key}={'ON' if value else 'OFF'}" for key, value in options.items()
    )

    option_template = Template(profiles[args.profile].command)  # type: ignore
    command_to_execute = option_template.render(options=options_str)

    logger.debug(f"Executing command: {command_to_execute}")
    os.system(command_to_execute)


if __name__ == "__main__":
    main()
